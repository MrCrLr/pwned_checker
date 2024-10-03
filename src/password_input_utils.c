#include "password_input_utils.h"

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

//  ----- Not printing characters and escape sequence handling -----
void escape_sequence_handler(char keystroke) {
    if (keystroke == 27) { // Escape character
        char next1 = getchar();
        
        // Check if we have a valid escape sequence starting
        if (next1 == '[') {
            char next2 = getchar(); // Moved outside of if-block for correct scope
            
            // Arrow keys (A: up, B: down, C: right, D: left), Page Up/Down, and Delete
            if (next2 == 'A' || next2 == 'B' || next2 == 'C' || next2 == 'D' || 
                next2 == 'F' || next2 == 'H' || next2 == '5' || next2 == '6' || next2 == '3') {
                
                if (next2 == '3') getchar();  // Consume the extra '~' for the Delete key
                return;  // Early exit after handling the escape sequence
            } else {
                // Not a known escape sequence; put characters back in the buffer
                ungetc(next2, stdin);
            }
        }

        // Not an escape sequence; put the first character back in the buffer
        ungetc(next1, stdin);
    } 
}

// Function to securely free password memory
void secure_free(char* buffer, int size) {
    if (buffer != NULL) {
        memset(buffer, 0, size); // Overwrite the memory area with zeros
        free(buffer); // Then free the memory
    }
}