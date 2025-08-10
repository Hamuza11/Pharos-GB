# ST7789 Pico-GB Implementation Summary

This directory contains a complete implementation of the Peanut-GB Game Boy emulator for the Raspberry Pi Pico with an ST7789 display.

## What Was Created

### 1. ST7789 Display Driver (`mk_st7789.h` and `mk_st7789.c`)
- **Purpose**: Hardware abstraction layer for the ST7789 display
- **Features**: 
  - Display initialization and configuration
  - Window and address setting for efficient drawing
  - Pixel writing with SPI optimization
  - Basic graphics primitives (fill, rectangles, text)
  - Support for 240x240 resolution (configurable)

### 2. Main Emulator Application (`main.c`)
- **Purpose**: Full Game Boy emulator using the ST7789 driver
- **Features**:
  - Dual-core operation (Core 0: emulator, Core 1: display)
  - Game Boy screen centered on 240x240 display
  - Classic Game Boy color palette
  - GPIO input handling for Game Boy controls
  - Overclocked to 266MHz for performance

### 3. Test Program (`test_st7789.c`)
- **Purpose**: Independent verification of the ST7789 driver
- **Features**:
  - Multiple test patterns (colors, bars, checkerboard, gradient, text, animation)
  - No emulator dependencies
  - Useful for debugging display issues

### 4. Build System
- **CMakeLists.txt**: Main build file for the emulator
- **CMakeLists_test.txt**: Build file for the test program
- **build.sh**: Automated build script for both programs
- **Makefile**: Alternative build method using make
- **check_sdk.sh**: Environment verification script

### 5. Documentation
- **README.md**: Comprehensive setup and usage instructions
- **SUMMARY.md**: This overview document

## Key Technical Features

### Display Integration
- **Resolution**: 240x240 ST7789 display
- **Game Boy Screen**: 160x144 centered with black borders
- **Color Format**: RGB565 (16-bit color)
- **Communication**: SPI interface at 30MHz

### Performance Optimizations
- **Multicore**: Dedicated core for display updates
- **SPI Efficiency**: 16-bit data transfers
- **Window Management**: Efficient screen region updates
- **Overclocking**: 266MHz operation for smooth emulation

### Hardware Abstraction
- **GPIO Functions**: Reset, chip select, data/command, backlight control
- **SPI Functions**: 8-bit and 16-bit data writing
- **Timing Functions**: Millisecond delays
- **Configurable**: Easy to adapt for different pin assignments

## Usage Workflow

### 1. Environment Setup
```bash
# Check if your environment is ready
./check_sdk.sh

# If not ready, follow the instructions to install Pico SDK
```

### 2. Building
```bash
# Build both programs
./build.sh

# Or use make
make all

# Build only specific components
make emulator    # Main emulator
make test        # Test program
```

### 3. Testing
```bash
# First, test the display with the test program
# Copy build_test/st7789_test.uf2 to your Pico
# Verify display shows test patterns

# If display works, try the full emulator
# Copy build_emulator/st7789_pico_gb.uf2 to your Pico
```

## Hardware Requirements

- **Microcontroller**: Raspberry Pi Pico or Pico W
- **Display**: ST7789 (240x240 or 240x135 resolution)
- **Connections**: SPI (MOSI, CLK, CS, DC, RST, BLK)
- **Power**: 3.3V supply

## Pin Assignments

| Function | GPIO | Description |
|----------|------|-------------|
| CS       | 17   | Chip Select |
| CLK      | 18   | SPI Clock   |
| MOSI     | 19   | SPI Data    |
| DC       | 20   | Data/Command|
| RST      | 21   | Reset       |
| BLK      | 22   | Backlight   |

## Customization Options

### Display Resolution
Modify `SCREEN_SIZE_X` and `SCREEN_SIZE_Y` in `mk_st7789.h`

### Color Palette
Edit the `init_palette()` function in `main.c` for different Game Boy color schemes

### Pin Assignments
Change GPIO definitions in `main.c` and `test_st7789.c`

### ROM Loading
Currently includes a test ROM. To load actual Game Boy ROMs:
1. Enable SD card support (`ENABLE_SDCARD=1`)
2. Implement ROM loading from SD card or flash
3. Modify `gb_rom_read()` function

## Troubleshooting

### Common Issues
1. **Display not working**: Check pin connections and power supply
2. **Poor performance**: Verify overclocking and SPI configuration
3. **Compilation errors**: Ensure Pico SDK is properly installed

### Debug Tools
- **USB Serial**: Monitor output via USB connection
- **Test Program**: Isolate display issues from emulator logic
- **Build Scripts**: Automated environment verification

## Next Steps

This implementation provides a solid foundation for:
- **Game ROM Loading**: Add SD card or flash storage support
- **Sound Support**: Integrate audio output
- **Save States**: Implement game save functionality
- **Additional Displays**: Adapt for other ST7789 variants
- **Performance Tuning**: Further optimize for specific use cases

## License

This implementation follows the same MIT License as the main Peanut-GB project.