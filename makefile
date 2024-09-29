# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -I/opt/homebrew/opt/openssl@3/include -I/opt/homebrew/include -Iinclude
LDFLAGS = -L/opt/homebrew/opt/openssl@3/lib -L/opt/homebrew/opt/xxhash/lib -lssl -lcrypto -lxxhash

# Directories
SRC_DIR = src
INCLUDE_DIR = include
RESOURCES_DIR = resources

# Target executable name
TARGET = pwned_checker

# Source files
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/password_input.c \
       $(SRC_DIR)/bloom_filter.c \
       $(SRC_DIR)/signal_handler.c \
       $(SRC_DIR)/utils.c \

# Object files (derived from source files)
OBJS = $(SRCS:.c=.o)

# Default target: Compile the executable
all: $(TARGET)

# Rule to build the target executable from object files
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Rule to compile each .c file into an object file
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up compiled files
clean:
	rm -f $(OBJS) $(TARGET)

# Usage message
.PHONY: all clean