# ==========================================
# BME280 Driver Makefile
# ==========================================

# 1. Compiler Settings
CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -g

# 2. File and Directory Settings
# Your specific executable name
TARGET = c_therm

SRC_DIR = src
OBJ_DIR = obj

# This automatically finds src/therm.c and any other .c files in src/
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Creates object paths like: obj/therm.o
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# ==========================================
# Build Rules
# ==========================================

all: $(TARGET)

# Link the object files into the final executable
$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	$(CC) $(OBJS) -o $@

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Create the object directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# ==========================================
# Utility Rules
# ==========================================

run: $(TARGET)
	./$(TARGET)

clean:
	@echo "Cleaning up..."
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean run
