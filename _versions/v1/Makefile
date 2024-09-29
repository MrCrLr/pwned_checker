# Compiler and flags
CC = gcc
CFLAGS = -Wall -I/opt/homebrew/opt/openssl@3/include
LDFLAGS = -L/opt/homebrew/opt/openssl@3/lib -lssl -lcrypto

# Target executable name
TARGET = cracker

# Source file
SRC = cracker.c

# Default target: Compile the executable
all: $(TARGET)

# Rule to build the target executable directly from the source file
$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

# Clean up compiled files
clean:
	rm -f $(TARGET)

# Usage message for Phony targets
.PHONY: all clean