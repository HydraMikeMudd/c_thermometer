# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -g

# Output executable name
OUTPUT_NAME = c_therm

# Default target main file (without extension)
# Usage: make TARGET=therm_with_display
TARGET ?= therm

# Directories
SRC_DIR = src
INC_DIR = include
DRIVER_DIR = epaper_driver
BUILD_DIR = build

# Source paths
# 1. All .c files in src/ EXCLUDING the main targets (therm.c and therm_with_display.c)
#    We filter them out specifically to avoid multiple definitions of 'main'.
COMMON_SRCS = $(filter-out $(SRC_DIR)/therm.c $(SRC_DIR)/therm_with_display.c, $(wildcard $(SRC_DIR)/*.c))

# 2. All .c files in the driver subdirectories
DRIVER_SRCS = $(wildcard $(DRIVER_DIR)/Config/*.c) \
              $(wildcard $(DRIVER_DIR)/e-Paper/*.c) \
              $(wildcard $(DRIVER_DIR)/Fonts/*.c) \
              $(wildcard $(DRIVER_DIR)/GUI/*.c)

# 3. The specific main file chosen by the TARGET variable
MAIN_SRC = $(SRC_DIR)/$(TARGET).c

# Combine all sources
SRCS = $(COMMON_SRCS) $(DRIVER_SRCS) $(MAIN_SRC)

# Generate object file names in the build directory, mirroring source structure
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)

# Include paths (add all directories that contain .h files)
INCLUDES = -I$(INC_DIR) \
           -I$(DRIVER_DIR)/Config \
           -I$(DRIVER_DIR)/e-Paper \
           -I$(DRIVER_DIR)/Fonts \
           -I$(DRIVER_DIR)/GUI

# --- Rules ---

# Default rule: build the executable
all: $(OUTPUT_NAME)

# Link the executable
$(OUTPUT_NAME): $(OBJS)
	@echo "Linking $@"
	$(CC) $(OBJS) -o $@

# Compile source files into object files
# This generic rule handles .c -> .o for any directory structure
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean up build artifacts
clean:
	@echo "Cleaning up..."
	rm -rf $(BUILD_DIR) $(OUTPUT_NAME)

# Helper to debug variable values
debug:
	@echo "Target Main: $(MAIN_SRC)"
	@echo "Common Srcs: $(COMMON_SRCS)"
	@echo "Driver Srcs: $(DRIVER_SRCS)"

.PHONY: all clean debug
