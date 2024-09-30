#ifndef PROGRAM_H
#define PROGRAM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <openssl/sha.h>
#include <ctype.h>
#include <termios.h>
#include <errno.h>

// Memory allocation size for initial and incremental allocation
#define PASSWORD_ALLOCATION 20

// Function prototypes
char* get_password_input(int max_password_length); // Get password input from the user
int read_and_mask_password(char **password, int *index, int *size); // Read password with masking
char* resize_buffer(char *buffer, int *size); // Resize buffer for password storage
void sha1_to_hex_string(const unsigned char *hash, char *output); // Convert SHA1 hash to hex string
void signal_handler(int signum); // Handle signals (e.g., Ctrl+C)
void cleanup(); // Cleanup resources
int set_echo(int enable); // Enable/disable password echoing

#endif