#include "password_input.h"

// Function to modify terminal settings
void set_input_mode(int enable) {
    static struct termios oldt, newt;

    if (enable == 0) {
        // Get the terminal settings
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;

        // Disable canonical mode and echoing
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    } else {
        // Restore the old settings
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
}

char get_yes_no_response() {
    set_input_mode(0); // Disable echoing and canonical mode for immediate response handling
    
    printf("Do you want to check another password? (y/n): ");
    char response;

    while (1) { // Infinite loop until valid input is received
        fflush(stdout);  // Ensure the prompt is shown immediately

        response = getchar(); // Get the first character

        if (response == 'y' || response == 'n' || response == 'Y' || response == 'N') {
            printf("%c\n", response);
            break; // Exit loop on valid input
        }
    }
    set_input_mode(1); // Restore terminal settings
    return tolower(response); // Normalize the response to lowercase and return
}