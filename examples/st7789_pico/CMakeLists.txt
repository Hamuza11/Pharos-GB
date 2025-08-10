cmake_minimum_required(VERSION 3.13)

# Initialize the SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(st7789_pico_gb C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Add the Peanut-GB header
add_library(peanut_gb INTERFACE)
target_include_directories(peanut_gb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../../)

# Create the main executable
add_executable(st7789_pico_gb
    main.c
    mk_st7789.c
)

# Enable USB output, disable UART output
pico_enable_stdio_usb(st7789_pico_gb 1)
pico_enable_stdio_uart(st7789_pico_gb 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(st7789_pico_gb)

# Link libraries
target_link_libraries(st7789_pico_gb
    pico_stdlib
    pico_multicore
    hardware_spi
    hardware_gpio
    hardware_clocks
    hardware_timer
    hardware_vreg
    peanut_gb
)

# Set compiler flags
target_compile_options(st7789_pico_gb PRIVATE
    -DENABLE_LCD=1
    -DENABLE_SOUND=0
    -DENABLE_SDCARD=0
    -DPEANUT_GB_HIGH_LCD_ACCURACY=1
    -DPEANUT_GB_USE_BIOS=0
)

# Set include directories
target_include_directories(st7789_pico_gb PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../  # For peanut_gb.h
)