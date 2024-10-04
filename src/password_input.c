#include "password_input.h"
#include "utils.h"

// Function to resize password buffer
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

// Function to securely free password memory
void secure_free(char* buffer, int size) {
    if (buffer != NULL) {
        memset(buffer, 0, size); // Overwrite the memory area with zeros
        free(buffer); // Then free the memory
    }
}

// Prompts user for pw and returns input – calls other functions to mask input
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

// Asterisk/Password Hide Function
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
        // Function handling escape sequences (Arrow keys, Page Up/Down, and Delete key)
        escape_sequence_handler(ch);

        // Handle backspace (ASCII 127 for backspace, '\b' for Ctrl+H backspace)
        if (ch == 127 || ch == '\b') {
            if (*index > 0) {
                (*index)--;           // Reduce the index to remove the character
                printf("\b \b");      // Erase the last character on the terminal
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

// Checks for non-printing characters and handles escape sequences
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

char get_yes_no_response() {
    set_echo(0); // Disable echoing and canonical mode for immediate response handling
    
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
    set_echo(1); // Restore terminal settings
    return tolower(response); // Normalize the response to lowercase and return
}