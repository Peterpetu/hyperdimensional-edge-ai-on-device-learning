#!/bin/bash
# =============================================================================
# Code Coverage Generation Script
# =============================================================================
# Usage: ./scripts/coverage.sh
# Generates HTML and text coverage reports using gcov/gcovr
#
# METHODOLOGY:
#   1. Clean previous coverage data (.gcda, .gcno files)
#   2. Run tests with coverage instrumentation (pio test -e coverage)
#      - PlatformIO's test command builds AND runs tests
#      - Unity framework provides main(), our HDC code is the library under test
#   3. Use gcovr to process coverage data and generate reports
#
# RATIONALE:
#   We use `pio test` instead of `pio run` because:
#   - Our HDC module is a library, not a standalone program
#   - Unity test framework provides the main() function
#   - `pio test` handles building with test sources + running
# =============================================================================

set -e

echo "=============================================="
echo "Generating Code Coverage Report"
echo "=============================================="

# Navigate to project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
cd "$PROJECT_ROOT"

# -----------------------------------------------------------------------------
# Step 1: Clean previous coverage data AND rebuild
# -----------------------------------------------------------------------------
# CRITICAL: .gcno (notes) and .gcda (data) files must be from the SAME build.
# Stale files cause "stamp mismatch" errors. We must:
#   1. Clean all coverage artifacts
#   2. Clean the PlatformIO build
#   3. Rebuild fresh with coverage instrumentation
# -----------------------------------------------------------------------------
echo "[1/4] Cleaning previous coverage data..."
find . -name "*.gcda" -delete 2>/dev/null || true
find . -name "*.gcno" -delete 2>/dev/null || true
rm -rf docs/coverage 2>/dev/null || true

echo "[2/4] Cleaning PlatformIO coverage build..."
pio run -e coverage -t clean 2>/dev/null || true

# -----------------------------------------------------------------------------
# Step 3: Run tests with coverage instrumentation
# -----------------------------------------------------------------------------
# NOTE: We use `pio test -e coverage` which:
#   - Compiles src/hdc/*.c with --coverage flags (generates .gcno files)
#   - Compiles test files and links with Unity
#   - Runs the test executable (generates .gcda files)
#   - Both .gcno and .gcda must be from this SAME build
# -----------------------------------------------------------------------------
echo "[3/4] Running tests with coverage instrumentation..."
if ! pio test -e coverage -v; then
    echo "WARNING: Tests failed or coverage build had issues"
    echo "Attempting to generate partial coverage report..."
fi

# -----------------------------------------------------------------------------
# Step 4: Generate coverage reports
# -----------------------------------------------------------------------------
echo "[4/4] Generating coverage report..."
mkdir -p docs/coverage

# -----------------------------------------------------------------------------
# Clean up library coverage files (Unity, etc.)
# We only want coverage of OUR code, not third-party libraries
# This prevents gcovr errors about missing Unity source files
# -----------------------------------------------------------------------------
echo "Removing third-party library coverage data..."
find .pio/build/coverage -path "*/lib*" -name "*.gcda" -delete 2>/dev/null || true
find .pio/build/coverage -path "*/lib*" -name "*.gcno" -delete 2>/dev/null || true

# Check if any coverage data was generated for OUR source
GCDA_COUNT=$(find .pio/build/coverage -path "*/src/*" -name "*.gcda" 2>/dev/null | wc -l)
if [ "$GCDA_COUNT" -eq 0 ]; then
    echo "ERROR: No coverage data generated for src/ (.gcda files not found)"
    echo "This usually means the tests didn't run or coverage instrumentation failed."
    exit 1
fi

echo "Found $GCDA_COUNT source coverage data files"

# -------------------------------------------------------------------------
# Generate coverage report
# -------------------------------------------------------------------------
# PlatformIO embeds absolute paths in coverage data, which causes issues
# with gcovr. We use lcov as the primary tool (better path handling),
# with gcovr as fallback.
# -------------------------------------------------------------------------

echo "DEBUG: Coverage files found:"
find .pio/build/coverage -name "*.gcda" 2>/dev/null | head -10
echo ""

if command -v lcov &> /dev/null; then
    echo "Using lcov for coverage analysis..."

    # Capture coverage data
    echo "Step 1: Capturing coverage data..."
    lcov --capture \
         --directory .pio/build/coverage \
         --output-file docs/coverage/coverage.info \
         --rc lcov_branch_coverage=1 \
         2>&1 | grep -v "^geninfo" || true

    # Check what we captured
    if [ -f docs/coverage/coverage.info ]; then
        echo "Captured coverage data. Files included:"
        grep "^SF:" docs/coverage/coverage.info | head -10
        echo ""
    else
        echo "ERROR: Failed to capture coverage data"
        exit 1
    fi

    # Extract only HDC source files (keep src/hdc, remove everything else)
    echo "Step 2: Filtering to HDC source files only..."
    lcov --extract docs/coverage/coverage.info \
         '*/src/hdc/*' \
         --output-file docs/coverage/coverage_filtered.info \
         --rc lcov_branch_coverage=1 \
         2>&1 | grep -v "^Reading" || true

    # Check if filtering worked
    if [ ! -s docs/coverage/coverage_filtered.info ]; then
        echo "WARNING: Filtering removed all data. Using unfiltered coverage."
        cp docs/coverage/coverage.info docs/coverage/coverage_filtered.info
    fi

    # Generate HTML report
    if command -v genhtml &> /dev/null && [ -s docs/coverage/coverage_filtered.info ]; then
        echo "Step 3: Generating HTML coverage report..."
        genhtml docs/coverage/coverage_filtered.info \
                --output-directory docs/coverage \
                --title "HDC Module - Code Coverage Report" \
                --rc lcov_branch_coverage=1 \
                2>&1 | tail -5 || echo "WARNING: HTML generation had issues"
    fi

    # Generate text summary
    echo ""
    echo "Step 4: Coverage Summary"
    echo "========================"
    lcov --summary docs/coverage/coverage_filtered.info \
         --rc lcov_branch_coverage=1 \
         2>&1 | grep -E "lines|functions|branches" || true

    # Also show per-file breakdown
    echo ""
    echo "Coverage by file:"
    echo "-----------------"
    lcov --list docs/coverage/coverage_filtered.info 2>/dev/null | grep -E "\.c|TOTAL" || true

elif command -v gcovr &> /dev/null; then
    echo "Using gcovr for coverage analysis..."

    # gcovr approach: run from build directory to resolve paths
    pushd .pio/build/coverage > /dev/null

    echo "Generating coverage report..."
    gcovr --root "$PROJECT_ROOT" \
          --filter '.*hdc.*' \
          --gcov-ignore-errors=source_not_found \
          --gcov-ignore-errors=no_working_dir_found \
          --html-details "$PROJECT_ROOT/docs/coverage/index.html" \
          --html-title "HDC Module - Code Coverage Report" \
          --txt \
          2>&1 | tee "$PROJECT_ROOT/docs/coverage/summary.txt" || echo "WARNING: Report generation had issues"

    popd > /dev/null
else
    echo "WARNING: Neither lcov nor gcovr installed."
    echo "Install with: apt install lcov"

    # Fallback: run gcov directly and parse output
    echo "Running gcov directly..."
    pushd .pio/build/coverage/src/hdc > /dev/null 2>&1 || true
    gcov *.gcda 2>/dev/null | tee "$PROJECT_ROOT/docs/coverage/summary.txt" || true
    popd > /dev/null 2>&1 || true
fi

echo ""
echo "=============================================="
echo "Coverage Report Generated"
echo "=============================================="
echo "HTML Report: docs/coverage/index.html"
echo "Text Summary: docs/coverage/summary.txt"
echo ""

# Show summary if available
if [ -f "docs/coverage/summary.txt" ] && [ -s "docs/coverage/summary.txt" ]; then
    echo "Coverage Summary:"
    echo "----------------------------------------------"
    cat docs/coverage/summary.txt
else
    echo "No coverage summary available."
    exit 1
fi
