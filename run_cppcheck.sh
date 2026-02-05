#!/bin/bash
# CPPCHECK script for Mayhem firmware with correct include paths

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_DIR/build/cppcheck"

# Create output directory
mkdir -p "$BUILD_DIR"

# CPPCHECK configuration
CPPCHECK_CMD="cppcheck --enable=all \
    --xml-version=2 \
    --suppressions-list=$PROJECT_DIR/.cppcheck_suppressions.txt \
    --inline-suppr \
    --std=c++11 \
    -I $PROJECT_DIR/firmware/application \
    -I $PROJECT_DIR/firmware/baseband \
    -I $PROJECT_DIR/firmware/common \
    -I $PROJECT_DIR/firmware/ui \
    -I $PROJECT_DIR/firmware/usb \
    -I $PROJECT_DIR/firmware/portapack \
    -I $PROJECT_DIR/firmware/hackrf/firmware \
    -I $PROJECT_DIR/firmware/chibios/os/hal/include \
    -I $PROJECT_DIR/firmware/chibios/os/hal/ports/common/ARMCMx \
    -I $PROJECT_DIR/firmware/chibios/os/kernel/include \
    -I $PROJECT_DIR/firmware/chibios/common/ports/ARMCMx \
    -I $PROJECT_DIR/firmware/chibios/common/oslib/include \
    -I $PROJECT_DIR/firmware/chibios/common/mcuconf \
    -I $PROJECT_DIR/firmware/chibios/common/halconf \
    -D'__weak=__attribute__((weak))' \
    -D'__attribute__(x)=' \
    -D'__CC__' \
    -D'__GNUC__' \
    --platform=unix64 \
    $PROJECT_DIR/firmware/application/apps/enhanced_drone_analyzer"

echo "Running CPPCHECK on Enhanced Drone Analyzer..."
echo "Command: $CPPCHECK_CMD"
echo ""

# Run cppcheck
$CPPCHECK_CMD 2>&1 | tee "$BUILD_DIR/cppcheck_output.log"

# Generate XML report
$CPPCHECK_CMD --xml 2>&1 | tee "$BUILD_DIR/cppcheck_result.xml"

echo ""
echo "CPPCHECK complete!"
echo "Results saved to: $BUILD_DIR/cppcheck_result.xml"
