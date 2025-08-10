#!/bin/bash

# Build script for ST7789 Pico-GB examples

echo "Building ST7789 Pico-GB examples..."

# Check if PICO_SDK_PATH is set
if [ -z "$PICO_SDK_PATH" ]; then
    echo "Error: PICO_SDK_PATH environment variable is not set"
    echo "Please set it to your Pico SDK installation path"
    echo "Example: export PICO_SDK_PATH=/path/to/pico-sdk"
    exit 1
fi

# Build the main emulator
echo "Building main emulator..."
mkdir -p build_emulator
cd build_emulator
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
cd ..

# Build the test program
echo "Building test program..."
mkdir -p build_test
cd build_test
cmake .. -f ../CMakeLists_test.txt -DCMAKE_BUILD_TYPE=Release
make -j4
cd ..

echo ""
echo "Build complete!"
echo ""
echo "Files created:"
echo "  Main emulator: build_emulator/st7789_pico_gb.uf2"
echo "  Test program: build_test/st7789_test.uf2"
echo ""
echo "To test the display:"
echo "  1. Copy st7789_test.uf2 to your Pico first"
echo "  2. Check that the display shows test patterns"
echo "  3. If working, copy st7789_pico_gb.uf2 for the full emulator"