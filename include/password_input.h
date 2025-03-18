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

/**
 * Struct representing a buffer that holds password securely.
 * Includes a pointer to a dynamically allocated buffer and an integer representing
 * the size of the buffer. 
 *
 * Components:
 * - buffer (char*): A pointer to the character array that holds the sensitive data.
 * - size (int): The size of the buffer in bytes. 
 */
typedef struct {
    unsigned char* buffer;
    int size;
} SecureBuffer;

unsigned char* resize_buffer(unsigned char *buffer, int *size); // Resize buffer for password length

SecureBuffer get_password_input(int max_password_length); // Get password input with maximum length
int read_and_mask_password(unsigned char **password, int *index, int *size); // Reads and hides user input
void escape_sequence_handler(char keystroke); // Checks for non-printing characters and handles escape sequences
char get_yes_no_response(); // Returns whether user want to check another password

void secure_free(unsigned char* buffer, int size); // Function to securely free password memory

#endif