#include "password_input.h"
#include "utils.h"

/**
 * Resizes a given buffer to accommodate additional data.
 *
 * This function is designed to dynamically increase the size of a password buffer
 * when its current capacity is exceeded. The buffer's size is increased by a
 * factor defined by PASSWORD_ALLOCATION. It uses `realloc` to adjust the size,
 * allowing for additional data without losing existing data.
 *
 * Parameters:
 * - buffer (char*): Pointer to the current buffer whose size needs to be increased.
 * - size (int*): Pointer to the current size of the buffer. This value will be
 *   updated to reflect the new size after resizing.
 *
 * Return:
 * - char*: Pointer to the resized buffer. Returns NULL if the memory allocation
 *   fails, after freeing the original buffer to avoid memory leaks.
 *
 * Note:
 * - It's crucial to check the return value of this function for NULL to ensure
 *   that the memory allocation was successful before proceeding with further
 *   operations on the buffer.
 */
unsigned char* resize_buffer(unsigned char *buffer, int *size) {
    *size += PASSWORD_ALLOCATION; // Increase the current size by allocation
    unsigned char *new_buffer = realloc(buffer, *size * sizeof(unsigned char));
    if (new_buffer == NULL) {
        fprintf(stderr, "Memory reallocation failed for buffer!\n");
        free(buffer);
        return NULL;
    }
    return new_buffer;
}

/**
 * Securely frees memory allocated for sensitive data.
 *
 * This function ensures that sensitive data such as passwords are wiped from
 * memory before the memory is freed. This is done by overwriting the memory
 * area with zeros before calling `free`, reducing the risk of sensitive data
 * being accessible in memory after deallocation.
 *
 * Parameters:
 * - buffer (char*): Pointer to the memory to be freed.
 * - size (int): Size of the buffer to be cleared.
 *
 * Note:
 * - This function should be used instead of `free` for all buffers that
 *   hold sensitive information to enhance security.
 */
void secure_free(unsigned char* buffer, int size) {
    if (buffer != NULL) {
        memset(buffer, 0, size); // Overwrite with zeros to clear sensitive data
        free(buffer); // Free the memory
    }
}

/**
 * Prompts the user to enter a password and returns the securely stored input.
 *
 * This function handles user input for passwords, ensuring the input is masked (not displayed),
 * stored securely, and dynamically resized if the initial buffer is not sufficient.
 * The function will repeatedly prompt the user until a valid password is entered or
 * the entered password exceeds the specified maximum length.
 *
 * Parameters:
 * - max_password_length (int): Maximum acceptable password length. If the entered
 *   password exceeds this length, the user is prompted to enter a shorter password.
 *
 * Returns:
 * - SecureBuffer: A struct containing the entered password and its length. If an
 *   error occurs during input (e.g., memory allocation fails), the size attribute
 *   of the returned struct is set to 0.
 *
 * Usage:
 * - The returned SecureBuffer must be checked to ensure its size is not 0 before
 *   using the password. The buffer should be securely freed using `secure_free`
 *   when no longer needed.
 */
SecureBuffer get_password_input(int max_password_length) {
    SecureBuffer secureBuf;
    secureBuf.size = PASSWORD_ALLOCATION; // Initial buffer size
    secureBuf.buffer = malloc(secureBuf.size * sizeof(unsigned char));

    if (secureBuf.buffer == NULL) {
        fprintf(stderr, "Memory allocation failed for password input!\n");
        secureBuf.size = 0; // Indicate failure
        return secureBuf;
    }

    int index = 0;
    // Main loop to handle password input and re-prompts
    do {
        printf("Enter your password: ");
        fflush(stdout);
        set_echo(0); // Disable echoing for password input

        // Call the password masking function
        int result = read_and_mask_password(&secureBuf.buffer, &index, &secureBuf.size);
        set_echo(1); // Re-enable echoing
        printf("\n");

        // Handle potential errors
        if (result == -1) {
            fprintf(stderr, "An error occurred during password input.\n");
            secure_free(secureBuf.buffer, secureBuf.size);
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

/**
 * Reads and masks password input from the user in a secure manner.
 *
 * This function prompts the user to enter their password, which is then read one character
 * at a time. Each character is masked by an asterisk (*) to prevent password visibility
 * when typed into a terminal. The function handles special characters such as backspace
 * for user corrections and ignores non-printable characters except those in the printable
 * ASCII range (33 to 126). It also manages dynamic buffer resizing to accommodate passwords
 * longer than the initial buffer size.
 *
 * Escape sequences (like arrow keys, page up/down, and delete) are processed and ignored
 * to prevent them from being included in the password buffer.
 *
 * Input:
 * - **password**: A pointer to a character array where the password will be stored.
 * - **index**: A pointer to the current position in the password buffer.
 * - **size**: A pointer to the total size of the password buffer.
 *
 * Output:
 * - The function returns 0 on successful capture of a non-empty password.
 * - Returns -1 if a memory reallocation fails during input handling.
 * - Returns -2 if no valid characters were input (the buffer remains empty).
 *
 * Note:
 * The function uses getchar() to read input, and thus operates on a character-by-character
 * basis, suitable for console applications where line buffering is disabled.
 */
int read_and_mask_password(unsigned char **password, int *index, int *size) {
    char ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        // If the buffer is full, reallocate more space
        if (*index >= *size - 1) { 
            unsigned char* new_buffer = resize_buffer(*password, size);
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