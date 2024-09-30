#include "deep_check.h"
#include "program.h"

int main() 
{
    // Initialize SQLite for deep check
    sqlite3 *db;
    if (init_db(&db, "database/pwnedpasswords.db") != 0)
        return 1;

    // Register cleanup function and signal handlers
    atexit(cleanup); 
    signal(SIGINT, signal_handler);  // Handle Ctrl+C
    signal(SIGTERM, signal_handler); // Handle termination signals

    char answer;
    const int maxPasswordLength = 100; // Max length for password input

    do {
        // Get password input via function + hide password while typing
        char *password = get_password_input(maxPasswordLength);
        if (password == NULL) 
        {
            printf("Failed to get password input.\n");
            break;
        }

        // Compute SHA1 hash of the password
        unsigned char hash[SHA_DIGEST_LENGTH];  // SHA1 hash is 20 bytes
        SHA1((unsigned char*)password, strlen(password), hash);
        
        // Convert SHA1 hash to hex string for database lookup
        char hash_hex[41]; // 40 hex characters + null terminator
        sha1_to_hex_string(hash, hash_hex);
        
        // DEBUGGING: CHECKING FULL_HASH STRING
        printf("Generated hash: %s\n", hash_hex);
        
        // Perform the deep check by searching the full SHA1 hash in the database
        if (deep_check_password(db, hash_hex) != 0) {
            printf("Error occurred during deep check.\n");
        }

        // Free password memory
        free(password);

        // Ask user if they want to check another password
        do {
            printf("Do you want to check another password? (y/n): ");
            answer = tolower(getchar());

            while (getchar() != '\n');  // Clear the input buffer
            
            if (answer == 'n') 
            {
                printf("Exiting program.\n");
                break;  // Exit the loop and terminate the program
            } 
            else if (answer != 'y') 
            {
                printf("Invalid input. Please enter 'y' or 'n'.\n");
            }
        } while (answer != 'y' && answer != 'n');

    } while (answer == 'y');

    // Close the database
    sqlite3_close(db);

    return 0;
}