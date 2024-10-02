#ifndef PASSWORD_INPUT_UTILS_H
#define PASSWORD_INPUT_UTILS_H

#include <termios.h>   // Terminal control
#include <unistd.h>    // For STDIN_FILENO
#include <string.h>    // For memset
#include <stdlib.h>    // For malloc and free
#include <stdio.h>     // For getchar and ungetc

void set_input_mode(int enable); // Function to modify terminal settings
void escape_sequence_handler(char keystroke); // Handle non-printing characters and escape sequences
void secure_free(char* buffer, int size); // Function to securely free password memory

#endif