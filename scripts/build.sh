#!/bin/bash
# =============================================================================
# Build Script for Arduino Uno (ATmega328P)
# =============================================================================
# Usage: ./scripts/build.sh
# =============================================================================

set -e

echo "=============================================="
echo "Building for Arduino Uno (ATmega328P)"
echo "=============================================="

# Navigate to project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
cd "$PROJECT_ROOT"

# Build
pio run -e uno

# Report size
echo ""
echo "=============================================="
echo "Memory Usage Report"
echo "=============================================="
pio run -e uno -t size 2>&1 | grep -E "(RAM|Flash|Program|Data):" || true

echo ""
echo "Build complete!"
