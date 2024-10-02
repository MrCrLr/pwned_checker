#include "program.h"

// Function to securely free password memory
void secure_free(char* buffer, int size) {
    if (buffer != NULL) {
        memset(buffer, 0, size); // Overwrite the memory area with zeros
        free(buffer); // Then free the memory
    }
}
// Function to get password input with maximum length
SecureBuffer get_password_input(int max_password_length) {
    SecureBuffer secureBuf;
    secureBuf.size = PASSWORD_ALLOCATION;
    secureBuf.buffer = malloc(secureBuf.size * sizeof(char));

    if (secureBuf.buffer == NULL) {
        fprintf(stderr, "Memory allocation failed for password input!\n");
        secureBuf.size = 0; // Set size to 0 to indicate failure
        return secureBuf;
    }

    int index = 0;
    do {
        printf("Enter your password: ");
        fflush(stdout);
        set_echo(0); // Disable echoing to hide password input

        // Call the password masking function
        int result = read_and_mask_password(&secureBuf.buffer, &index, &secureBuf.size);
        set_echo(1); // Re-enable echoing
        printf("\n");

        // Check for errors
        if (result == -1) {
            fprintf(stderr, "An error occurred during password input.\n");
            secure_free(secureBuf.buffer, secureBuf.size); // Securely free the password
            secureBuf.buffer = NULL;
            secureBuf.size = 0;
            return secureBuf;
        } 
        else if (index == 0) {
            printf("Password cannot be empty. Please try again.\n");
        } 
        else if (index > max_password_length) {
            printf("Password exceeds (%d characters). Please try again.\n", max_password_length);
            index = 0; // Reset index to allow re-entry
        }
    } while (index == 0 || index > max_password_length); // Repeat until valid input

    secureBuf.buffer[index] = '\0'; // Null-terminate the string
    return secureBuf; // Return the SecureBuffer with the password
}

// ----- Asterisk/Password Hide Function -----
int read_and_mask_password(char **password, int *index, int *size) {
    char ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        // If the buffer is full, reallocate more space
        if (*index >= *size - 1) { 
            char* new_buffer = resize_buffer(*password, size);
            if (new_buffer == NULL) {
                fprintf(stderr, "Memory reallocation failed!\n");
                secure_free(*password, *size); // Securely free the old buffer
                return -1;
            }
            *password = new_buffer;
        }

        // Handle escape sequences (Arrow keys, Page Up/Down, and Delete key)
        if (ch == 27) { // Escape character
            char next1 = getchar();
            char next2 = getchar();
            
            // Arrow keys (A: up, B: down, C: right, D: left), Page Up/Down, and Delete
            if (next1 == '[' && (next2 == 'A' || next2 == 'B' || next2 == 'C' || next2 == 'D' || 
            next2 == 'F' || next2 == 'H' || next2 == '5' || next2 == '6' || next2 == '3')) {
                // Arrow keys, Page Up/Down, Delete detected, ignore and continue
                if (next2 == '3') getchar();  // Consume the extra '~' for Delete key
                continue;
            } else {
                // Not a known escape sequence; put characters back in the buffer
                ungetc(next2, stdin);
                ungetc(next1, stdin);
            }
        }

        // Handle backspace (ASCII 127 for backspace, '\b' for Ctrl+H backspace)
        if (ch == 127 || ch == '\b') {
            if (*index > 0) {
                (*index)--;           // Reduce the index to remove the character
                printf("\b \b");      // Erase the last character on the terminal
            }
        }
        // Handle UTF-8 multibyte characters (allow UTF-8 input for special chars like é, ä, etc.)
        else if ((unsigned char)ch >= 128) {
            (*password)[(*index)++] = ch;  // Store the first byte of the multibyte sequence
            printf("*");
            int remaining_bytes = 0;

            // Check how many more bytes are in the multibyte sequence
            if ((ch & 0xE0) == 0xC0) remaining_bytes = 1;        // 2-byte sequence
            else if ((ch & 0xF0) == 0xE0) remaining_bytes = 2;   // 3-byte sequence
            else if ((ch & 0xF8) == 0xF0) remaining_bytes = 3;   // 4-byte sequence

            // Collect the rest of the multibyte sequence
            for (int i = 0; i < remaining_bytes; i++) {
                ch = getchar();
                (*password)[(*index)++] = ch;
            }
        }
        // Allow only printable ASCII characters between 33 ('!') and 126 ('~')
        else if (ch >= 33 && ch <= 126) {
            (*password)[(*index)++] = ch;
            printf("*");  // Mask input with asterisk
        }
        // Ignore spaces (ASCII 32)
        else if (ch == ' ') {
            continue;  // Ignore spaces
        }
    }
    
    // Ensure that we return a failure code if no valid characters were input
    return *index > 0 ? 0 : -2; // Return -2 if no valid input was collected, otherwise success
}

// Function to resize buffer
char* resize_buffer(char *buffer, int *size) {
    *size *= PASSWORD_ALLOCATION; // Increase buffer size
    char *new_buffer = realloc(buffer, *size * sizeof(char));
    if (new_buffer == NULL) {
        fprintf(stderr, "Memory reallocation failed for buffer!\n");
        free(buffer);
        return NULL;
    }
    return new_buffer;
}

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
    char response;

    set_input_mode(0); // Disable echoing and canonical mode for immediate response handling
    
    printf("Do you want to check another password? (y/n): ");
    fflush(stdout);  // Make sure the prompt is shown immediately

    response = getchar();
    printf("%c\n", response);

    while (response != 'y' && response != 'Y' && response != 'n' && response != 'N') {
        printf("Invalid input. Please enter 'y' or 'n': ");
        fflush(stdout);  // Ensure the prompt is displayed again before re-input

        response = getchar();
        printf("%c\n", response);
    }

    set_input_mode(1); // Restore terminal settings

    return tolower(response); // Normalize the response to lowercase and return
}