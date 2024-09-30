#ifndef PROGRAM_H
#define PROGRAM_H

#include <ctype.h>
#include <errno.h>
#include <openssl/sha.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define ALLOCATION 20 // Memory allocation size for inital and incremental allocation

// Password input functions
char* get_password_input(int max_password_length);
int read_and_mask_password(char **password, int *index, int *size);
char* resize_buffer(char *buffer, int *size);

// Password processing for match in bloom filter
void get_hash_prefix(unsigned char *hash, char *prefix);

// Helper function to convert SHA1 hash to hex string
void sha1_to_hex_string(const unsigned char *hash, char *output);

// Hiding password while typed
void signal_handler(int signum);
void cleanup();
int set_echo(int enable);

#endif