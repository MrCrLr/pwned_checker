# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -fsanitize=address -I../include -I/opt/homebrew/opt/openssl@3/include -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/opt/openssl@3/lib -L/opt/homebrew/opt/xxhash/lib -lssl -lcrypto -lxxhash -lsqlite3 -fsanitize=address

# Directories
SRC_DIR = ../src
DATABASE_DIR = ../database
BIN_DIR = ../bin

# Target executable name
TARGET = $(BIN_DIR)/pwned_checker

# Source files
SRCS = $(SRC_DIR)/main.c \
	   $(SRC_DIR)/password_input_utils.c \
       $(SRC_DIR)/password_input.c \
       $(SRC_DIR)/utils.c \
       $(SRC_DIR)/deep_check.c \
       $(DATABASE_DIR)/create_database.c

# Object files (derived from source files)
OBJS = $(SRCS:.c=.o)

# Default target: Compile the executable
all: $(TARGET)

# Rule to build the target executable from object files
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Rule to compile each .c file into an object file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up compiled files
clean:
	rm -f $(OBJS) $(TARGET)

# Usage message
.PHONY: all clean