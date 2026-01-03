# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -g -O -ffunction-sections -fdata-sections -DDEBUG

# Output executable name
OUTPUT_NAME = c_therm

# 1. CHANGED: Default is now therm_with_display
TARGET ?= therm_with_display

# Directories
SRC_DIR = src
INC_DIR = include
DRIVER_DIR = src/epaper_driver
BUILD_DIR = build

# Base Includes
INCLUDES = -I$(INC_DIR)

# Base Sources (Generic src files, excluding the mains)
COMMON_SRCS = $(filter-out $(SRC_DIR)/therm.c $(SRC_DIR)/therm_with_display.c, $(wildcard $(SRC_DIR)/*.c))
SRCS = $(COMMON_SRCS)

# Libraries variable
LIBS =

# --- CONDITIONAL LOGIC ---
ifeq ($(TARGET),therm_with_display)
    # MACROS: Using LGPIO now
    CFLAGS += -D RPI -D USE_LGPIO_LIB
    
    # LIBRARIES: Link lgpio
    LIBS += -Wl,--gc-sections -llgpio -lm
    
    # DRIVER SOURCES
    DRIVER_CONFIG = $(DRIVER_DIR)/Config/DEV_Config.c \
                    $(DRIVER_DIR)/Config/dev_hardware_SPI.c
    
    DRIVER_ASSETS = $(wildcard $(DRIVER_DIR)/Fonts/*.c) \
                    $(wildcard $(DRIVER_DIR)/GUI/*.c)

    # Explicit driver file selection for EPD
    DRIVER_EPD = $(DRIVER_DIR)/e-Paper/EPD_2in13_V4.c

    SRCS += $(DRIVER_CONFIG) $(DRIVER_ASSETS) $(DRIVER_EPD)
    
    # INCLUDES
    INCLUDES += -I$(DRIVER_DIR)/Config \
                -I$(DRIVER_DIR)/e-Paper \
                -I$(DRIVER_DIR)/Fonts \
                -I$(DRIVER_DIR)/GUI
endif

# Main Source file based on target
MAIN_SRC = $(SRC_DIR)/$(TARGET).c
SRCS += $(MAIN_SRC)

# Generate object file names
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)

# --- Rules ---

# Default target (runs when you type 'make')
all: $(OUTPUT_NAME)

# 2. NEW SHORTCUTS
# "make display" forces TARGET=therm_with_display
display:
	$(MAKE) TARGET=therm_with_display
	@echo "✅ Build complete: ./c_therm (with display support)"

# "make headless" forces TARGET=therm
headless:
	$(MAKE) TARGET=therm
	@echo "✅ Build complete: ./c_therm (sensor only)"

# Link the executable
$(OUTPUT_NAME): $(OBJS)
	@echo "Linking $@ (Target: $(TARGET))"
	$(CC) $(OBJS) -o $@ $(LIBS)

# Compile source files
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "Cleaning up..."
	rm -rf $(BUILD_DIR) $(OUTPUT_NAME)

debug:
	@echo "Target:  $(TARGET)"
	@echo "Sources: $(SRCS)"

.PHONY: all clean debug display headless
