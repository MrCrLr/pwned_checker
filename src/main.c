#include "bloom_filter.h"
#include "program.h"

int main() 
{
    // Open the pwned password file and determine file size
    size_t file_size = get_file_size("resources/pwnedpasswords.txt");
    if (file_size == 0) 
    {
        fprintf(stderr, "Could not determine file size.\n");
        return 1;
    }

    // Determine the number of Bloom filters needed based on the file size
    int num_filters = determine_optimal_bloom_filters(file_size);

    // Create an array of Bloom filters
    BloomFilter **filters = malloc(num_filters * sizeof(BloomFilter*));
    for (int i = 0; i < num_filters; i++) {
        filters[i] = create_bloom_filter(BLOOM_SIZE);
    }

    // Populate the filters with pwned passwords
    split_into_bloom_filters("resources/pwnedpasswords.txt", filters, num_filters);

    // Register cleanup function and signal handlers
    atexit(cleanup); 
    signal(SIGINT, signal_handler);  // Handle Ctrl+C
    signal(SIGTERM, signal_handler); // Handle termination signals

    char answer;
    
    do {
        // Get password input via function + hide password while typing
        char *password = get_password_input();

        if (password == NULL) 
        {
            printf("Failed to get password input.\n");
            break;
        }

        // Compute SHA1 hash of the password
        unsigned char hash[SHA_DIGEST_LENGTH];  // SHA1 hash is 20 bytes
        SHA1((unsigned char*)password, strlen(password), hash);  // Convert the first 5 bytes of hash to hexadecimal
        
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
            printf("Password found in pwned list!\n");
        } 
        else 
        {
            printf("Password not found in pwned list.\n");
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

    // Free allocated memory for Bloom filters
    for (int i = 0; i < num_filters; i++) 
    {
        free_bloom_filter(filters[i]);
    }
    free(filters);

    return 0;
}