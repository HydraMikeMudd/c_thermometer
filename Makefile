# ==========================================
# Settings
# ==========================================

CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -lwiringPi

# ==========================================
# Directories & Files
# ==========================================

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

# TARGET is now just the name, meaning it saves to the current dir
TARGET = therm_monitor

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# ==========================================
# Build Rules
# ==========================================

all: $(TARGET)

# Link the executable in the current directory
$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compile source files into the obj/ directory
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Only need to create the obj directory now
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean up objects and the specific executable file
clean:
	@echo "Cleaning up..."
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean
