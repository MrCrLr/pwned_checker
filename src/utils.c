#include "program.h"

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

void configure_terminal_for_immediate_input() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

void restore_terminal_settings() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag |= (ICANON | ECHO); // Enable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

char get_immediate_response() {
    configure_terminal_for_immediate_input();
    char response = getchar();
    restore_terminal_settings();
    return response;
}