#ifndef PROGRAM_H
#define PROGRAM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <openssl/sha.h>
#include <ctype.h>
#include <termios.h>
#include <errno.h>

// Memory allocation size for initial and incremental allocation
#define PASSWORD_ALLOCATION 20

typedef struct {
    char* buffer;
    int size;
} SecureBuffer;

// Function prototypes
void secure_free(char* buffer, int size);
SecureBuffer get_password_input(int max_password_length); // Get password input from the user
int read_and_mask_password(char **password, int *index, int *size); // Read password with masking
char* resize_buffer(char *buffer, int *size); // Resize buffer for password storage

void clear_input_buffer();
char get_yes_no_response(); // Get users response if they want to check another password

void signal_handler(int signum); // Handle signals (e.g., Ctrl+C)
void cleanup(); // Cleanup resources
int set_echo(int enable); // Enable/disable password echoing
void configure_terminal_for_immediate_input();
void restore_terminal_settings();
char get_immediate_response();


#endif