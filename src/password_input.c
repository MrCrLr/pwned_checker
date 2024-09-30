#include "program.h"

char* get_password_input() 
{
    int size = ALLOCATION;
    char *password = malloc(size * sizeof(char));
    if (password == NULL) 
    {
        fprintf(stderr, "Memory allocation failed!\n");
        return NULL;
    }

    int index = 0;
    int max_length = 100; // Set a maximum length for the password (adjust as necessary)

    do {
        printf("Enter your password: ");
        set_echo(0); // Disable echoing to hide password input

        // Call the password masking function
        int result = read_and_mask_password(&password, &index, &size);
        set_echo(1); // Re-enable echoing
        printf("\n");

        // Check if the password is too short or too long
        if (result == -1) {
            printf("An error occurred during password input.\n");
            free(password);
            return NULL;
        } 
        else if (index == 0) {
            printf("Password cannot be empty. Please try again.\n");
        } 
        else if (index > max_length) {
            printf("Password exceeds maximum allowed length (%d characters). Please try again.\n", max_length);
            index = 0; // Reset index to allow re-entry
        }

    } while (index == 0 || index > max_length); // Repeat until valid input

    password[index] = '\0'; // Null-terminate the string

    return password; // Return the dynamically allocated password
}

// ----- Asterisk/Password Hide Function -----
int read_and_mask_password(char **password, int *index, int *size) 
{
    char ch;
    while ((ch = getchar()) != '\n') 
    {
        // If the buffer is full, reallocate more space
        if (*index >= *size - 1) 
        { 
            *password = resize_buffer(*password, size);
            if (*password == NULL) 
            {
                fprintf(stderr, "Memory reallocation failed!\n");
                return -1;  // Return error without freeing, since realloc handles that
            }
        }
        
        // Handle backspace
        if (ch == 127 || ch == '\b') 
        {
            if (*index > 0) 
            {
                (*index)--;
                printf("\b \b"); // Erase the last character
            }
        } 
        // Allow only printable characters
        else if (isprint(ch)) 
        {
            (*password)[(*index)++] = ch;
            printf("*"); // Mask input with asterisk
        } 
        // Handle non-printable characters
        else 
        {
            printf("\nInvalid character entered. Only printable characters are allowed.\n");
            return -1;  // Return error if invalid character is encountered
        }
    }
    return 0; // Success
}

// Function to resize buffer
char* resize_buffer(char *buffer, int *size) 
{
    *size *= ALLOCATION;
    char *new_buffer = realloc(buffer, *size * sizeof(char));
    if (new_buffer == NULL) 
    {
        fprintf(stderr, "Memory reallocation failed!\n");
        free(buffer);
        return NULL;
    }
    return new_buffer;
}