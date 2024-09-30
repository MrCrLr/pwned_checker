#include "program.h"

/**
 * Retrieves the hash prefix from a SHA1 hash.
 * 
 * @param hash: The SHA1 hash (20 bytes).
 * @param prefix: The output buffer for the first 10 characters (5 bytes in hex).
 */
void get_hash_prefix(unsigned char *hash, char *prefix) {
    if (hash == NULL || prefix == NULL) {
        fprintf(stderr, "Error: Null pointer passed to get_hash_prefix.\n");
        return; // Handle null pointer
    }

    for (int i = 0; i < 5; i++) {
        snprintf(prefix + i * 2, 3, "%02X", hash[i]);
    }
    prefix[10] = '\0';  // Null-terminate the prefix
}

/**
 * Converts a SHA1 hash to a hex string.
 * 
 * @param hash: The SHA1 hash (20 bytes).
 * @param output: The output buffer for the hex string (40 characters + null terminator).
 */
void sha1_to_hex_string(const unsigned char *hash, char *output) {
    if (hash == NULL || output == NULL) {
        fprintf(stderr, "Error: Null pointer passed to sha1_to_hex_string.\n");
        return; // Handle null pointer
    }

    for (int i = 0; i < 20; i++) {
        snprintf(output + (i * 2), 3, "%02X", hash[i]);  // Convert each byte to hex
    }
    output[40] = '\0';  // Null-terminate the string
}

/**
 * Sets the terminal echo mode.
 * 
 * @param enable: If non-zero, enables echo; otherwise, disables it.
 * @return: 0 on success, -1 on error.
 */
int set_echo(int enable) {
    struct termios tty;
    
    // Get the current terminal attributes
    if (tcgetattr(STDIN_FILENO, &tty) != 0) {
        fprintf(stderr, "Error getting terminal attributes: %s\n", strerror(errno));
        return -1; // Return error code
    }
    
    if (!enable) {
        tty.c_lflag &= ~(ECHO | ICANON); // Disable echo and canonical mode
    } else {
        tty.c_lflag |= (ECHO | ICANON);   // Enable echo and canonical mode
    }
    
    // Set the modified terminal attributes
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) != 0) {
        fprintf(stderr, "Error setting terminal attributes: %s\n", strerror(errno));
        return -1; // Return error code
    }
    
    return 0; // Success
}

void signal_handler(int signum) {
    set_echo(1); // Re-enable echoing before handling the signal
    exit(signum); // Exit with the received signal code
}

void cleanup() {
    set_echo(1); // Re-enable echoing before the program exits
}