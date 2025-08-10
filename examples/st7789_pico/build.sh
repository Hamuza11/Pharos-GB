#!/bin/bash

# Build script for ST7789 Pico-GB examples
# Make sure PICO_SDK_PATH is set before running this script

set -e

echo "Building ST7789 Pico-GB examples..."

# Check if PICO_SDK_PATH is set
if [ -z "$PICO_SDK_PATH" ]; then
    echo "Error: PICO_SDK_PATH environment variable is not set"
    echo "Please set it to your Pico SDK installation path"
    echo "Example: export PICO_SDK_PATH=/path/to/pico-sdk"
    exit 1
fi

echo "Using Pico SDK at: $PICO_SDK_PATH"

# Build the test program
echo "Building ST7789 test program..."
mkdir -p build_test
cd build_test
cmake -f ../CMakeLists_test.txt ..
make -j4
cd ..

# Build the main emulator program
echo "Building ST7789 Pico-GB emulator..."
mkdir -p build_emulator
cd build_emulator
cmake -f ../CMakeLists.txt ..
make -j4
cd ..

echo "Build completed successfully!"
echo ""
echo "Output files:"
echo "  Test program: build_test/st7789_test.uf2"
echo "  Emulator: build_emulator/st7789_pico_gb.uf2"
echo ""
echo "To test the display driver, copy st7789_test.uf2 to your Pico first."
echo "To run the full emulator, copy st7789_pico_gb.uf2 to your Pico."