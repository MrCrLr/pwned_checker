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
            printf("Password exceeds maximum allowed length (%d characters). Please try again.\n", max_password_length);
            index = 0; // Reset index to allow re-entry
        }
    } while (index == 0 || index > max_password_length); // Repeat until valid input

    secureBuf.buffer[index] = '\0'; // Null-terminate the string
    return secureBuf; // Return the SecureBuffer with the password
}

// ----- Asterisk/Password Hide Function -----
int read_and_mask_password(char **password, int *index, int *size) {
    int ch;
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
        
        // Handle backspace
        if (ch == 127 || ch == '\b') {
            if (*index > 0) {
                (*index)--;
                printf("\b \b"); // Erase the last character
            }
        } 
        // Allow only printable characters, excluding space
        else if (isprint(ch) && ch != ' ') {
            (*password)[(*index)++] = ch;
            printf("*"); // Mask input with asterisk
        } 
        // Ignore spaces completely, without breaking or printing errors
        else if (ch == ' ') {
            continue;  // Skip spaces completely
        }
        // Handle other non-printable characters
        else {
            continue;  // Skip other non-printable characters
        }
    }
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