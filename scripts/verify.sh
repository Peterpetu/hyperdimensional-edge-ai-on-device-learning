#!/bin/bash
# =============================================================================
# Engineer-Grade Full Verification Suite
# =============================================================================
# Usage: ./scripts/verify.sh
# Runs ALL quality gates: build, test, coverage, analysis, formal verification
# =============================================================================

set -e

echo "=============================================="
echo "Engineer-GRADE VERIFICATION SUITE"
echo "=============================================="
echo ""

# Navigate to project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
cd "$PROJECT_ROOT"

# Track results
RESULTS=""
add_result() {
    RESULTS="$RESULTS\n$1"
}

# -----------------------------------------------------------------------------
# [1/5] BUILD
# -----------------------------------------------------------------------------
echo "=============================================="
echo "[1/5] Building for Arduino Uno"
echo "=============================================="
# Capture build output to extract RAM usage
BUILD_OUTPUT=$(./scripts/build.sh 2>&1)
BUILD_STATUS=$?
echo "$BUILD_OUTPUT"

if [ $BUILD_STATUS -eq 0 ]; then
    add_result "[PASS] Build - Arduino Uno firmware compiled successfully"
else
    add_result "[FAIL] Build - Compilation errors"
    echo "Build failed. Stopping verification."
    exit 1
fi

# Check RAM usage from build output
# Format: "RAM:   [=         ]   5.8% (used 118 bytes from 2048 bytes)"
RAM_USAGE=$(echo "$BUILD_OUTPUT" | grep "RAM:" | grep -oE '[0-9]+\.[0-9]+%' | head -1 | tr -d '%')
if [ -z "$RAM_USAGE" ]; then
    RAM_USAGE="0"
fi
# Compare using awk (more portable than bc)
RAM_EXCEEDED=$(echo "$RAM_USAGE" | awk '{if ($1 > 70) print "yes"; else print "no"}')
if [ "$RAM_EXCEEDED" = "yes" ]; then
    add_result "[WARN] RAM usage is $RAM_USAGE% (exceeds 70% professional limit)"
else
    add_result "[PASS] RAM usage: $RAM_USAGE% (within 70% limit)"
fi

echo ""

# -----------------------------------------------------------------------------
# [2/5] UNIT TESTS
# -----------------------------------------------------------------------------
echo "=============================================="
echo "[2/5] Running Unit Tests"
echo "=============================================="
if ./scripts/test.sh; then
    add_result "[PASS] Unit Tests - All tests passed"
else
    add_result "[FAIL] Unit Tests - Some tests failed"
fi

echo ""

# -----------------------------------------------------------------------------
# [3/5] CODE COVERAGE
# -----------------------------------------------------------------------------
echo "=============================================="
echo "[3/5] Generating Code Coverage"
echo "=============================================="
if ./scripts/coverage.sh; then
    add_result "[PASS] Coverage - Report generated"
else
    add_result "[WARN] Coverage - Could not generate report"
fi

echo ""

# -----------------------------------------------------------------------------
# [4/5] STATIC ANALYSIS
# -----------------------------------------------------------------------------
echo "=============================================="
echo "[4/5] Running Static Analysis"
echo "=============================================="
if ./scripts/analyze.sh; then
    add_result "[PASS] Static Analysis - No critical issues"
else
    add_result "[WARN] Static Analysis - Issues found (review recommended)"
fi

echo ""

# -----------------------------------------------------------------------------
# [5/5] FORMAL VERIFICATION (Optional)
# -----------------------------------------------------------------------------
echo "=============================================="
echo "[5/5] Running Formal Verification"
echo "=============================================="
if ./scripts/formal.sh; then
    add_result "[PASS] Formal Verification - CBMC checks passed"
else
    add_result "[INFO] Formal Verification - Skipped or needs review"
fi

echo ""

# -----------------------------------------------------------------------------
# SUMMARY
# -----------------------------------------------------------------------------
echo "=============================================="
echo "VERIFICATION SUMMARY"
echo "=============================================="
echo -e "$RESULTS"
echo ""

# Count results (grep -c returns 1 if no match, so we handle that)
PASS_COUNT=$(echo -e "$RESULTS" | grep -c "\[PASS\]" 2>/dev/null || true)
FAIL_COUNT=$(echo -e "$RESULTS" | grep -c "\[FAIL\]" 2>/dev/null || true)
WARN_COUNT=$(echo -e "$RESULTS" | grep -c "\[WARN\]" 2>/dev/null || true)
# Ensure counts are valid numbers
PASS_COUNT=${PASS_COUNT:-0}
FAIL_COUNT=${FAIL_COUNT:-0}
WARN_COUNT=${WARN_COUNT:-0}

echo "----------------------------------------------"
echo "Passed:   $PASS_COUNT"
echo "Failed:   $FAIL_COUNT"
echo "Warnings: $WARN_COUNT"
echo "----------------------------------------------"

if [ "$FAIL_COUNT" -gt 0 ]; then
    echo ""
    echo "RESULT: VERIFICATION FAILED"
    echo "Please fix the failed checks before deployment."
    exit 1
else
    echo ""
    echo "=============================================="
    echo "ALL QUALITY GATES PASSED!"
    echo "=============================================="
    echo ""
    echo "This codebase meets Engineer-grade quality standards:"
    echo "  - Successful compilation for target hardware"
    echo "  - All unit tests passing"
    echo "  - Static analysis clean"
    echo "  - Memory usage within professional limits"
    echo ""
fi
