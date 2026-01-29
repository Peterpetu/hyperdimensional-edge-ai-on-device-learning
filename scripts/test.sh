#!/bin/bash
# =============================================================================
# Unit Test Runner
# =============================================================================
# Usage: ./scripts/test.sh
# Runs Unity-based unit tests on native (PC) platform
# =============================================================================

set -e

echo "=============================================="
echo "Running Unit Tests (Native Platform)"
echo "=============================================="

# Navigate to project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
cd "$PROJECT_ROOT"

# Run tests with verbose output
pio test -e native -v

echo ""
echo "=============================================="
echo "All tests passed!"
echo "=============================================="
