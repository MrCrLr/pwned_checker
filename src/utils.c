#include "utils.h"

/**
 *  Sets the terminal echo mode. 
 * 
 *  Parameter enable: If non-zero, enables echo; otherwise, disables it.
 *  Return: 0 on success, -1 on error.
 * 
 * 	tty.c_lflag: This is the “local flags” field of the termios structure. 
 *  It controls various terminal settings.
 * 
 *	Bitwise AND (&=) and Bitwise OR (|=): These operations are manipulating  
 *  specific bits in the c_lflag field that correspond to the ECHO and ICANON flags.
 * 
 *	Disabling: The expression tty.c_lflag &= ~(ECHO | ICANON) 
 *  clears the ECHO and ICANON bits (disabling echo and canonical mode).
 * 
 *	Enabling: The expression tty.c_lflag |= (ECHO | ICANON) 
 *  sets the ECHO and ICANON bits (enabling echo and canonical mode).
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