# Testing the ST7789 Driver

Before running the full Game Boy emulator, it's recommended to test the ST7789 display driver to ensure it's working correctly.

## Quick Test

1. **Build the test program:**
   ```bash
   cd examples/st7789_pico
   ./build.sh
   ```

2. **Copy the test program to your Pico:**
   - Copy `build_test/st7789_test.uf2` to your Pico
   - The Pico will restart and begin running the test patterns

3. **What to expect:**
   - The display should show various test patterns:
     - Solid color fills (red, green, blue, white, black)
     - Color bars
     - Checkerboard pattern
     - Gradient pattern
     - Text display
     - Animated moving circles

## Manual Testing

If you prefer to build manually:

```bash
# Build test program only
mkdir build_test
cd build_test
cmake -f ../CMakeLists_test.txt ..
make -j4
cd ..

# Copy to Pico
cp build_test/st7789_test.uf2 /path/to/pico/
```

## Troubleshooting

### Display Not Working
- Check all pin connections (CS, CLK, MOSI, DC, RST, LED)
- Verify 3.3V power supply
- Check that the Pico is running (LED should blink)
- Use USB serial to see debug output

### Poor Performance
- Ensure the Pico is overclocked to 266MHz
- Check SPI clock frequency (should be 30MHz)
- Verify GPIO functions are set correctly

### Build Errors
- Make sure `PICO_SDK_PATH` is set correctly
- Ensure you have the latest Pico SDK
- Check that all required libraries are available

## Test Pattern Details

### Pattern 1: Solid Colors
Tests basic fill functionality with primary colors.

### Pattern 2: Color Bars
Tests rectangular drawing and color accuracy.

### Pattern 3: Checkerboard
Tests pixel-level drawing and pattern generation.

### Pattern 4: Gradient
Tests individual pixel setting and color interpolation.

### Pattern 5: Text
Tests the font rendering system.

### Pattern 6: Animation
Tests real-time drawing and frame updates.

## Next Steps

Once the test program works correctly:

1. **Test the full emulator:**
   ```bash
   cp build_emulator/st7789_pico_gb.uf2 /path/to/pico/
   ```

2. **Load a Game Boy ROM:**
   - The emulator currently includes a simple test ROM
   - For real ROMs, you'll need to implement SD card support

3. **Customize the setup:**
   - Modify GPIO pins if needed
   - Adjust display settings
   - Customize the color palette

## Hardware Verification

The test program will help verify:
- ✅ Display initialization
- ✅ SPI communication
- ✅ GPIO control
- ✅ Color rendering
- ✅ Drawing functions
- ✅ Real-time updates

If all test patterns display correctly, your ST7789 driver is working and ready for the full emulator!