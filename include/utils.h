#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>      // For printf, fprintf, stderr
#include <string.h>
#include <stdlib.h>     // For exit, atexit
#include <signal.h>     // For signal handling (signal, SIGINT, SIGTERM)
#include <termios.h>    // For terminal control (struct termios, tcgetattr, tcsetattr)
#include <unistd.h>     // For STDIN_FILENO (standard input file descriptor)
#include <errno.h>      // For error handling (errno, strerror)

// Function prototypes
void signal_handler(int signum); // Handle signals (e.g., Ctrl+C)
void cleanup(); // Cleanup resources
int set_echo(int enable); // Enable/disable password echoing

#endif