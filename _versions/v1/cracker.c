#include <openssl/sha.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define INITIAL_SIZE 10  // Initial size for password buffer
#define REALLOC_FACTOR 2 // How much to increase the buffer when needed

// Function Prototypes
char* get_password_input();
int read_and_mask_password(char *password, int *index, int *size);
void get_hash_prefix(unsigned char *hash, char *prefix);
void signal_handler(int signum);
void cleanup();
void set_echo(int enable);

int main() 
{
    // Register cleanup function and signal handlers
    atexit(cleanup); 
    signal(SIGINT, signal_handler); // Handle Ctrl+C
    signal(SIGTERM, signal_handler); // Handle termination signals

    // Get password input via function + hide password while typing
    char *password = get_password_input();
    if (password == NULL) 
    {
        fprintf(stderr, "Memory allocation failed!\n");
        return 1;
    }
    

    // For testing purposes only; remove this in production code.
    printf("\nYour password is: %s\n", password);


    // Password processing...
    // Compute SHA1 hash of the password
    unsigned char hash[SHA_DIGEST_LENGTH]; // defined as 20
    SHA1((unsigned char*)password, strlen(password), hash);
    
    char prefix[11]; // To store the first 5 characters (5 bytes = 10 hex digits + null terminator)
    get_hash_prefix(hash, prefix);

    // Print the hash in hexadecimal format
    printf("SHA1 hash is: ");
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) 
    {
        printf("%02x", hash[i]);
    }
    printf("\n");


    // Free allocated memory
    free(password);
    return 0;
}


// ----- Password Input Function -----
char* get_password_input() 
{
    int size = INITIAL_SIZE;  // Initial size buffer
    char *password = (char *)malloc(size * sizeof(char));
    if (password == NULL) 
    {
        fprintf(stderr, "Memory allocation failed!\n");
        return NULL;
    }

    int index = 0;

    printf("Enter your password: ");
    set_echo(0); // Disable echoing and use non-canonical mode

    // Call the password masking function
    if (read_and_mask_password(password, &index, &size) == -1) 
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
int read_and_mask_password(char *password, int *index, int *size) 
{
    char ch;
    while ((ch = getchar()) != '\n') 
    {
        // If the buffer is full, reallocate more space
        if (*index >= *size - 1) 
        { 
            *size *= REALLOC_FACTOR;
            char *new_password = realloc(password, *size * sizeof(char));
            if (new_password == NULL) 
            {
                fprintf(stderr, "Memory reallocation failed!\n");
                return -1;
            }
            password = new_password;
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
        else 
        {
            password[(*index)++] = ch;
            printf("*"); // Mask input
        }
    }
    return 0; // Success
}

void get_hash_prefix(unsigned char *hash, char *prefix) 
{
    sprintf(prefix, "%02x%02x%02x%02x%02x", hash[0], hash[1], hash[2], hash[3], hash[4]);
}

// ----- Signal Handling and Cleanup Functions -----
void signal_handler(int signum) 
{
    set_echo(1); // Re-enable echoing before handling the signal
    exit(signum); // Exit with the received signal code
}

void cleanup() 
{
    set_echo(1); // Re-enable echoing before the program exits
}

// ----- Helper Functions -----
void set_echo(int enable) 
{
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if (!enable) {
        tty.c_lflag &= ~(ECHO | ICANON); // Disable echo and canonical mode
    } else {
        tty.c_lflag |= (ECHO | ICANON); // Enable echo and canonical mode
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}