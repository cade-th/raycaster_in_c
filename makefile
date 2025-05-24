CC = clang
CFLAGS = -std=c99
# -Wall -Wextra
LDFLAGS = -lraylib -lm

# Directories
SRC_DIR = src
BUILD_DIR = build
LIB_DIR = lib

# Files
SRC_FILES = $(SRC_DIR)/*.c 

LIB_FILES = 

# Binaries
APP_BIN = $(BUILD_DIR)/app

# Default target
all: $(APP_BIN)

# App build
$(APP_BIN): $(SRC_FILES)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all test clean
	$(SRC_DIR)/error/error.c \

PHONY: all test clean
