# ST7789 Pico-GB

This example demonstrates how to run the Peanut-GB Game Boy emulator on a Raspberry Pi Pico with an ST7789 display.

## Hardware Requirements

- Raspberry Pi Pico or Pico W
- ST7789 display (240x240 or 240x135 resolution)
- Game Boy ROM file (optional, for testing)

## Pin Connections

Connect the ST7789 display to the Pico as follows:

| ST7789 Pin | Pico GPIO | Function |
|------------|-----------|----------|
| VCC        | 3.3V      | Power    |
| GND        | GND       | Ground   |
| SCL        | GPIO 18   | SPI Clock |
| SDA        | GPIO 19   | SPI MOSI |
| RES        | GPIO 21   | Reset    |
| DC         | GPIO 20   | Data/Command |
| CS         | GPIO 17   | Chip Select |
| BLK        | GPIO 22   | Backlight |

## Game Boy Controls

| Game Boy Button | Pico GPIO | Function |
|-----------------|-----------|----------|
| D-Pad Up       | GPIO 2    | Up       |
| D-Pad Down     | GPIO 3    | Down     |
| D-Pad Left     | GPIO 4    | Left     |
| D-Pad Right    | GPIO 5    | Right    |
| A Button       | GPIO 6    | A        |
| B Button       | GPIO 7    | B        |
| Select         | GPIO 8    | Select   |
| Start          | GPIO 9    | Start    |

## Building

1. Make sure you have the Raspberry Pi Pico SDK installed and the `PICO_SDK_PATH` environment variable set.

2. Clone this repository and navigate to the ST7789 example:
   ```bash
   cd examples/st7789_pico
   ```

3. **Recommended: Test the display driver first:**
   ```bash
   ./build.sh
   ```
   This will build both the test program and the emulator.

4. **Alternative: Build manually:**
   ```bash
   # Build test program first (recommended)
   mkdir build_test
   cd build_test
   cmake -f ../CMakeLists_test.txt ..
   make -j4
   cd ..
   
   # Build emulator
   mkdir build
   cd build
   cmake ..
   make -j4
   cd ..
   ```

5. The build will create several output files:
   - `build_test/st7789_test.uf2` - Test program for verifying display functionality
   - `build/st7789_pico_gb.uf2` - Full Game Boy emulator

## Usage

### Testing the Display Driver (Recommended First Step)

1. Connect your ST7789 display to the Pico using the pin connections above.

2. Copy the `build_test/st7789_test.uf2` file to your Pico.

3. The Pico will restart and begin running the test program.

4. You should see various test patterns on the display:
   - Solid colors (red, green, blue, white, black)
   - Color bars
   - Checkerboard pattern
   - Gradient pattern
   - Text display
   - Animated moving circles

5. If all patterns display correctly, your display driver is working!

### Running the Full Emulator

1. Once the test program works, copy the `build/st7789_pico_gb.uf2` file to your Pico.

2. The Pico will restart and begin running the Game Boy emulator.

3. The Game Boy screen will be centered on the 240x240 display with black borders.

## Features

- **Dual-core operation**: Core 0 runs the emulator, Core 1 handles display updates
- **SPI communication**: Fast SPI interface for smooth display updates
- **Centered display**: The 160x144 Game Boy screen is centered on the 240x240 display
- **Classic palette**: Uses the classic Game Boy green color palette
- **Overclocked**: Runs at 266MHz for better performance

## Customization

### Display Resolution
If you have a different ST7789 display resolution, modify the `SCREEN_SIZE_X` and `SCREEN_SIZE_Y` defines in `mk_st7789.h`.

### Color Palette
You can customize the Game Boy color palette by modifying the `init_palette()` function in `main.c`. The palette uses RGB565 format.

### ROM Loading
Currently, the example includes a simple test ROM. To load actual Game Boy ROMs, you'll need to:

1. Enable SD card support by setting `ENABLE_SDCARD` to 1
2. Implement proper ROM loading from SD card or flash storage
3. Modify the `gb_rom_read()` function to read from your ROM source

### Sound Support
To enable sound, set `ENABLE_SOUND` to 1 and implement the required audio functions. You'll also need to include the minigb_apu library.

## Troubleshooting

### Display Not Working
- Check all pin connections
- Verify the ST7789 is receiving 3.3V power
- Check that the reset and chip select pins are properly connected

### Poor Performance
- Ensure the Pico is running at full speed (266MHz)
- Check that SPI is configured for high frequency (30MHz)
- Verify that the display is properly initialized

### Compilation Errors
- Make sure `PICO_SDK_PATH` is set correctly
- Ensure you have the latest version of the Pico SDK
- Check that all required libraries are available

## Technical Details

### ST7789 Driver
The ST7789 driver implements the standard ST7789 command set and provides a simple interface for:
- Display initialization
- Window and address setting
- Pixel writing
- Basic graphics primitives

### Multicore Architecture
- **Core 0**: Runs the Game Boy emulator and handles input
- **Core 1**: Manages the display and handles LCD drawing commands
- **Communication**: Uses the Pico's FIFO for inter-core communication

### SPI Configuration
- SPI0 is used for communication
- 8-bit data format
- 30MHz clock frequency
- Mode 0 (CPOL=0, CPHA=0)

## License

This project is licensed under the MIT License, same as the main Peanut-GB project.

## Contributing

Feel free to submit issues and enhancement requests. The ST7789 driver is designed to be easily extensible for additional features.