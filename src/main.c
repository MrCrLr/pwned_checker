#include "bloom_filter.h"
#include "deep_check.h"
#include "program.h"

int main() 
{
    // Initialize SQLite for deep check
    sqlite3 *db;
    if (init_db(&db, "database/pwnedpasswords.db") != 0)
        return 1;

    // Get the number of entries in the database
    int file_size = get_password_count(db);
    if (file_size < 0) {
        fprintf(stderr, "Failed to get password count from database.\n");
        return 1;
    }

    // Determine the number of Bloom filters needed based on the file size
    int num_filters = determine_optimal_bloom_filters(file_size);

    // Create an array of Bloom filters
    BloomFilter **filters = malloc(num_filters * sizeof(BloomFilter*));
    for (int i = 0; i < num_filters; i++) 
    {
        filters[i] = create_bloom_filter(BLOOM_SIZE);
    }

    // Populate the filters with pwned passwords from the database
    populate_bloom_filters_from_db(filters, num_filters, db);

    // Close the database after populating the Bloom filters
    sqlite3_close(db);

    // Register cleanup function and signal handlers
    atexit(cleanup); 
    signal(SIGINT, signal_handler);  // Handle Ctrl+C
    signal(SIGTERM, signal_handler); // Handle termination signals

    char answer;
    int max_length_for_password = 100;

    do {
        // Get password input via function + hide password while typing
        char *password = get_password_input(max_length_for_password);

        if (password == NULL) 
        {
            printf("Failed to get password input.\n");
            break;
        }

        // Compute SHA1 hash of the password
        unsigned char hash[SHA_DIGEST_LENGTH];  // SHA1 hash is 20 bytes
        SHA1((unsigned char*)password, strlen(password), hash);
        
        char prefix[11];  // To store the first 5 characters (5 bytes = 10 hex digits + null terminator)
        get_hash_prefix(hash, prefix);

        // Check if password (or its prefix) is in any of the Bloom filters
        int found = 0;
        for (int i = 0; i < num_filters; i++) 
        {
            if (bloom_contains(filters[i], prefix)) 
            {
                found = 1;
                break;
            }
        }

        if (found) 
        {
            printf("Possible match found in Bloom filter, performing deep check...\n");
            
            // Convert SHA1 hash to hex string for database lookup
            char hash_hex[41]; // 40 hex characters + null terminator
            sha1_to_hex_string(hash, hash_hex);
            
            // DEBUGGING: CHECKING FULL_HASH STRING
            printf("Generated hash: %s\n", hash_hex);
            
            // Perform the deep check by searching the full SHA1 hash in the database
            // char known_hash[] = "00000000DD7F2A1C68A35673713783CA390C9E93"; // DEBUGGING
            deep_check_password("database/pwnedpasswords.db", prefix, hash_hex); // Change back to known_hash for debugging
        } 
        else printf("Password not found in pwned list.\n");

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

    // Free allocated memory for Bloom filters
    for (int i = 0; i < num_filters; i++) 
    {
        free_bloom_filter(filters[i]);
    }
    free(filters);

    return 0;
}