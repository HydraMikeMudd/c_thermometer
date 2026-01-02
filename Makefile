# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -g

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
# Only include the library and the driver source files if building the display version
ifeq ($(TARGET),therm_with_display)
    # 1. Add the BCM2835 library
    LIBS += -llgpio -lm
    
    # 2. Add the e-Paper driver sources
    # (We include them here because they likely depend on the library. 
    #  If we included them in the normal 'therm' build, the linker might error.)
    DRIVER_SRCS = $(wildcard $(DRIVER_DIR)/Config/*.c) \
                  $(wildcard $(DRIVER_DIR)/e-Paper/*.c) \
                  $(wildcard $(DRIVER_DIR)/Fonts/*.c) \
                  $(wildcard $(DRIVER_DIR)/GUI/*.c)
    
    SRCS += $(DRIVER_SRCS)
    
    # 3. Add driver headers to include path
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
	@echo "Target: $(TARGET)"
	@echo "Libs:   $(LIBS)"
	@echo "Sources: $(SRCS)"

.PHONY: all clean debug
