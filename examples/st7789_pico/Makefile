# Makefile for ST7789 Pico-GB examples

# Check if PICO_SDK_PATH is set
ifndef PICO_SDK_PATH
$(error PICO_SDK_PATH environment variable is not set. Please set it to your Pico SDK installation path)
endif

# Build directories
BUILD_EMU = build_emulator
BUILD_TEST = build_test

# Default target
all: emulator test

# Build the main emulator
emulator:
	@echo "Building main emulator..."
	@mkdir -p $(BUILD_EMU)
	@cd $(BUILD_EMU) && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4

# Build the test program
test:
	@echo "Building test program..."
	@mkdir -p $(BUILD_TEST)
	@cd $(BUILD_TEST) && cmake .. -f ../CMakeLists_test.txt -DCMAKE_BUILD_TYPE=Release && make -j4

# Clean build directories
clean:
	@echo "Cleaning build directories..."
	@rm -rf $(BUILD_EMU) $(BUILD_TEST)

# Clean and rebuild everything
rebuild: clean all

# Show help
help:
	@echo "Available targets:"
	@echo "  all      - Build both emulator and test program (default)"
	@echo "  emulator - Build only the main emulator"
	@echo "  test     - Build only the test program"
	@echo "  clean    - Remove build directories"
	@echo "  rebuild  - Clean and rebuild everything"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Files created:"
	@echo "  Main emulator: $(BUILD_EMU)/st7789_pico_gb.uf2"
	@echo "  Test program:  $(BUILD_TEST)/st7789_test.uf2"

.PHONY: all emulator test clean rebuild help