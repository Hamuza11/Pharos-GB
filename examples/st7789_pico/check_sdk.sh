#!/bin/bash

# Check Pico SDK setup script

echo "Checking Pico SDK setup..."

# Check if PICO_SDK_PATH is set
if [ -z "$PICO_SDK_PATH" ]; then
    echo "❌ PICO_SDK_PATH environment variable is not set"
    echo ""
    echo "Please set it to your Pico SDK installation path:"
    echo "  export PICO_SDK_PATH=/path/to/pico-sdk"
    echo ""
    echo "If you haven't installed the Pico SDK yet, you can:"
    echo "  1. Clone it: git clone https://github.com/raspberrypi/pico-sdk.git"
    echo "  2. Set the path: export PICO_SDK_PATH=/path/to/pico-sdk"
    echo "  3. Initialize submodules: cd \$PICO_SDK_PATH && git submodule update --init"
    exit 1
fi

echo "✅ PICO_SDK_PATH is set to: $PICO_SDK_PATH"

# Check if the path exists
if [ ! -d "$PICO_SDK_PATH" ]; then
    echo "❌ PICO_SDK_PATH directory does not exist: $PICO_SDK_PATH"
    exit 1
fi

echo "✅ PICO_SDK_PATH directory exists"

# Check for required files
if [ ! -f "$PICO_SDK_PATH/external/pico_sdk_import.cmake" ]; then
    echo "❌ pico_sdk_import.cmake not found in $PICO_SDK_PATH/external/"
    echo "The Pico SDK may not be properly initialized"
    exit 1
fi

echo "✅ pico_sdk_import.cmake found"

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo "❌ CMake is not installed or not in PATH"
    echo "Please install CMake (version 3.13 or higher)"
    exit 1
fi

echo "✅ CMake found: $(cmake --version | head -n1)"

# Check for make
if ! command -v make &> /dev/null; then
    echo "❌ Make is not installed or not in PATH"
    echo "Please install Make"
    exit 1
fi

echo "✅ Make found: $(make --version | head -n1)"

# Check for arm-none-eabi-gcc
if ! command -v arm-none-eabi-gcc &> /dev/null; then
    echo "⚠️  arm-none-eabi-gcc not found in PATH"
    echo "This is required for building Pico projects"
    echo "Please install the ARM GCC toolchain"
    echo ""
    echo "On Ubuntu/Debian: sudo apt install gcc-arm-none-eabi"
    echo "On macOS: brew install arm-none-eabi-gcc"
    echo "On Windows: Download from ARM website or use WSL"
    exit 1
fi

echo "✅ ARM GCC toolchain found: $(arm-none-eabi-gcc --version | head -n1)"

echo ""
echo "🎉 All checks passed! Your environment is ready to build Pico projects."
echo ""
echo "You can now run:"
echo "  ./build.sh          # Build both programs"
echo "  make                 # Build both programs (alternative)"
echo "  make test            # Build only the test program"
echo "  make emulator        # Build only the emulator"