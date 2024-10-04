#ifndef PASSWORD_INPUT_H
#define PASSWORD_INPUT_H

#include <stdio.h>       // For printf(), fprintf(), getchar(), etc.
#include <stdlib.h>      // For malloc(), free(), etc.
#include <string.h>      // For string manipulation, if needed (e.g., strncpy(), strcmp())
#include <ctype.h>       // For tolower(), isprint(), etc.
#include <termios.h>     // For terminal input manipulation (tcsetattr(), tcgetattr())
#include <unistd.h>      // For STDIN_FILENO constant


// Memory allocation size for initial and incremental allocation
#define PASSWORD_ALLOCATION 20

typedef struct {
    char* buffer;
    int size;
} SecureBuffer;

char* resize_buffer(char *buffer, int *size); // Resize buffer for password length

SecureBuffer get_password_input(int max_password_length); // Get password input with maximum length
int read_and_mask_password(char **password, int *index, int *size); // Asterisk/Password Hide Function
void escape_sequence_handler(char keystroke); // Checks for non-printing characters and handles escape sequences
char get_yes_no_response(); // Returns whether use want to check another password

void secure_free(char* buffer, int size); // Function to securely free password memory

#endif