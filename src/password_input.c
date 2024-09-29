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

    printf("Enter your password: ");
    set_echo(0); // Disable echoing and use non-canonical mode

    // Call the password masking function
    if (read_and_mask_password(&password, &index, &size) == -1) 
    {
        free(password);
        return NULL;
    }

    set_echo(1); // Re-enable echoing
    password[index] = '\0'; // Null-terminate the string
    printf("\n");

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
                free(password);
                return -1;
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
        else if (isprint(ch))
        {
            (*password)[(*index)++] = ch;
            printf("*"); // Mask input
        }
        // Ignore non-printable characters silently (optional, or you could handle them here)
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