# Compiler settings
CC = gcc
# Added -DDEBUG to match original makefile
CFLAGS = -Wall -Wextra -g -O -ffunction-sections -fdata-sections -DDEBUG

# Output executable name
OUTPUT_NAME = c_therm

# Default target main file
TARGET ?= therm

# Directories
SRC_DIR = src
INC_DIR = include
DRIVER_DIR = epaper_driver
BUILD_DIR = build

# Base Includes
INCLUDES = -I$(INC_DIR)

# Base Sources (Generic src files, excluding the mains)
COMMON_SRCS = $(filter-out $(SRC_DIR)/therm.c $(SRC_DIR)/therm_with_display.c, $(wildcard $(SRC_DIR)/*.c))
SRCS = $(COMMON_SRCS)

# Libraries variable (starts empty)
LIBS =

# --- CONDITIONAL LOGIC ---
ifeq ($(TARGET),therm_with_display)
    # 1. MACROS (Crucial Fix)
    # The original makefile defines these to tell the code to use the Pi hardware and BCM lib.
    CFLAGS += -D RPI -D USE_BCM2835_LIB
    
    # 2. LIBRARIES
    # -lbcm2835: The GPIO library
    # -lm: Math library
    # --gc-sections: Cleans up unused code
    LIBS += -Wl,--gc-sections -lbcm2835 -lm
    
    # 3. DRIVER SOURCES
    # We must be specific here. The original makefile only picks specific Config files
    # for the BCM2835 mode. If we wildcard *.c in Config, we might pull in 
    # conflicting Jetson or Sysfs drivers.
    
    # A. Config Files (Matches "RPI_DEV" else block in original makefile)
    DRIVER_CONFIG = $(DRIVER_DIR)/Config/DEV_Config.c \
                    $(DRIVER_DIR)/Config/dev_hardware_SPI.c
    
    # B. Fonts and GUI (Safe to include all)
    DRIVER_ASSETS = $(wildcard $(DRIVER_DIR)/Fonts/*.c) \
                    $(wildcard $(DRIVER_DIR)/GUI/*.c)

    # C. E-Paper Driver (CRITICAL WARNING)
    # The original makefile picks ONE specific file (e.g., EPD_2in13_V3.c).
    # If your folder contains ALL the drivers, wildcarding *.c here will cause 
    # "Multiple Definition" errors.
    # If you have cleaned the folder to only have your screen's driver, keep this wildcard.
    # If not, replace the line below with your specific file, e.g.: $(DRIVER_DIR)/e-Paper/EPD_2in13_V3.c
    DRIVER_EPD = $(wildcard $(DRIVER_DIR)/e-Paper/*.c)

    SRCS += $(DRIVER_CONFIG) $(DRIVER_ASSETS) $(DRIVER_EPD)
    
    # 4. INCLUDES
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

all: $(OUTPUT_NAME)

$(OUTPUT_NAME): $(OBJS)
	@echo "Linking $@ (Target: $(TARGET))"
	$(CC) $(OBJS) -o $@ $(LIBS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "Cleaning up..."
	rm -rf $(BUILD_DIR) $(OUTPUT_NAME)

debug:
	@echo "Target:  $(TARGET)"
	@echo "Flags:   $(CFLAGS)"
	@echo "Sources: $(SRCS)"

.PHONY: all clean debug
