#ifndef PROGRAM_H
#define PROGRAM_H

#include <ctype.h>
#include <openssl/sha.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define ALLOCATION 20 // Memory allocation size for inital and incremental allocation

// Password input functions
char* get_password_input();
int read_and_mask_password(char **password, int *index, int *size);
char* resize_buffer(char *buffer, int *size);

// Password processing for match in database
void get_hash_prefix(unsigned char *hash, char *prefix);

// Signal handling
void signal_handler(int signum);
void cleanup();

// Utility functions
void set_echo(int enable);

#endif