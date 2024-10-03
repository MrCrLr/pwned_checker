#ifndef PROGRAM_H
#define PROGRAM_H

#include <stdio.h>      // For printf(), fprintf(), getchar()
#include <stdlib.h>     // For exit()
#include <string.h>      // For strlen
#include <termios.h>    // For terminal control (tcgetattr(), tcsetattr())
#include <unistd.h>     // For STDIN_FILENO (standard input file descriptor)
#include <signal.h>     // For signal handling (signal(), SIGINT, SIGTERM)
#include <errno.h>      // For errno and strerror() to display error messages
#include <openssl/sha.h> // For SHA-1 hashing
#include <sqlite3.h>     // For SQLite functions

// Function prototypes
void clear_input_buffer();
void signal_handler(int signum); // Handle signals (e.g., Ctrl+C)
void cleanup(); // Cleanup resources
int set_echo(int enable); // Enable/disable password echoing

#endif