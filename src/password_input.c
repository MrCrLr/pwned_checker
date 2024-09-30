#include "program.h"

// Function to get password input with maximum length
char* get_password_input(int max_password_length) {
    int current_buffer_size = ALLOCATION; 
    char *password = malloc(current_buffer_size * sizeof(char));
    if (password == NULL) {
        fprintf(stderr, "Memory allocation failed for password input!\n");
        return NULL;
    }
    int index = 0;

    do {
        printf("Enter your password: ");
        set_echo(0); // Disable echoing to hide password input

        // Call the password masking function
        int result = read_and_mask_password(&password, &index, &current_buffer_size);
        set_echo(1); // Re-enable echoing
        printf("\n");

        // Check for errors
        if (result == -1) {
            fprintf(stderr, "An error occurred during password input.\n");
            free(password);
            return NULL;
        } 
        else if (index == 0) {
            printf("Password cannot be empty. Please try again.\n");
        } 
        else if (index > max_password_length) {
            printf("Password exceeds maximum allowed length (%d characters). Please try again.\n", max_password_length);
            index = 0; // Reset index to allow re-entry
        }
    } while (index == 0 || index > max_password_length); // Repeat until valid input

    password[index] = '\0'; // Null-terminate the string
    return password; // Return the dynamically allocated password
}

// ----- Asterisk/Password Hide Function -----
int read_and_mask_password(char **password, int *index, int *size) {
    char ch;
    while ((ch = getchar()) != '\n') {
        // If the buffer is full, reallocate more space
        if (*index >= *size - 1) { 
            *password = resize_buffer(*password, size);
            if (*password == NULL) {
                fprintf(stderr, "Memory reallocation failed!\n");
                return -1;  // Return error without freeing
            }
        }
        
        // Handle backspace
        if (ch == 127 || ch == '\b') {
            if (*index > 0) {
                (*index)--;
                printf("\b \b"); // Erase the last character
            }
        } 
        // Allow only printable characters
        else if (isprint(ch)) {
            (*password)[(*index)++] = ch;
            printf("*"); // Mask input with asterisk
        } 
        // Handle non-printable characters
        else {
            printf("\nInvalid character entered. Only printable characters are allowed.\n");
            return -1;  // Return error if invalid character is encountered
        }
    }
    return 0; // Success
}

// Function to resize buffer
char* resize_buffer(char *buffer, int *size) {
    *size *= ALLOCATION; // Increase buffer size
    char *new_buffer = realloc(buffer, *size * sizeof(char));
    if (new_buffer == NULL) {
        fprintf(stderr, "Memory reallocation failed for buffer!\n");
        free(buffer);
        return NULL;
    }
    return new_buffer;
}