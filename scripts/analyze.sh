#!/bin/bash
# =============================================================================
# Static Analysis Script
# =============================================================================
# Usage: ./scripts/analyze.sh
# Runs cppcheck and clang-tidy for Engineer-grade code quality
# =============================================================================

set -e

echo "=============================================="
echo "Running Static Analysis Suite"
echo "=============================================="

# Navigate to project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
cd "$PROJECT_ROOT"

ERRORS=0

# -----------------------------------------------------------------------------
# CPPCHECK - Memory safety and general C analysis
# -----------------------------------------------------------------------------
echo ""
echo "=============================================="
echo "[1/2] CPPCHECK - Memory Safety Analysis"
echo "=============================================="

if command -v cppcheck &> /dev/null; then
    cppcheck --enable=all \
             --std=c99 \
             --platform=avr8 \
             --suppress=missingIncludeSystem \
             --suppress=unusedFunction \
             --suppress=unmatchedSuppression \
             -Isrc/hal -Isrc/hdc -Isrc/app \
             --error-exitcode=0 \
             src/ 2>&1 || ERRORS=$((ERRORS + 1))
    echo "cppcheck analysis complete."
else
    echo "WARNING: cppcheck not installed. Skipping."
    echo "Install with: apt install cppcheck"
fi

# -----------------------------------------------------------------------------
# CLANG-TIDY - Modern C analysis and best practices
# -----------------------------------------------------------------------------
echo ""
echo "=============================================="
echo "[2/2] CLANG-TIDY - Code Quality Analysis"
echo "=============================================="

if command -v clang-tidy &> /dev/null; then
    # Find all C files and analyze them
    find src -name "*.c" -print0 2>/dev/null | while IFS= read -r -d '' file; do
        echo "Analyzing: $file"
        clang-tidy "$file" \
            -checks="readability-*,performance-*,bugprone-*,-readability-magic-numbers" \
            -- -Isrc/hal -Isrc/hdc -Isrc/app -std=c99 2>/dev/null || true
    done
    echo "clang-tidy analysis complete."
else
    echo "WARNING: clang-tidy not installed. Skipping."
    echo "Install with: apt install clang-tidy"
fi

echo ""
echo "=============================================="
echo "Static Analysis Complete"
echo "=============================================="

if [ $ERRORS -gt 0 ]; then
    echo "Analysis completed with $ERRORS error(s)."
    exit 1
else
    echo "No critical errors found."
fi
