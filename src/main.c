#include "bloom_filter.h"
#include "program.h"

int main() 
{
    // Open the pwned password file
    FILE *pwned_file = fopen("resources/pwnedpasswords.txt", "r");
    if (!pwned_file) 
    {
        fprintf(stderr, "Could not open pwned password file.\n");
        return 1;
    }

    // Initialize Bloom Filter
    BloomFilter *filter = create_bloom_filter(BLOOM_SIZE);
    
    // Populate the filter with pwned passwords
    populate_bloom_filter(filter, pwned_file);
    fclose(pwned_file);

    // Register cleanup function and signal handlers
    atexit(cleanup); 
    signal(SIGINT, signal_handler); // Handle Ctrl+C
    signal(SIGTERM, signal_handler); // Handle termination signals

    // Get password input via function + hide password while typing
    char *password = get_password_input();

    if (password == NULL) 
    {
        printf("Failed to get password input.\n");
        return 1;
    }

    // Compute SHA1 hash of the password
    unsigned char hash[SHA_DIGEST_LENGTH]; // SHA1 hash is 20 bytes
    SHA1((unsigned char*)password, strlen(password), hash);  // Convert the first 5 bytes of hash to hexadecimal
    
    char prefix[11]; // To store the first 5 characters (5 bytes = 10 hex digits + null terminator)
    get_hash_prefix(hash, prefix);

    // Print the hash in hexadecimal format
    printf("SHA1 hash is: ");
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) 
    {
        printf("%02x", hash[i]);
    }
    printf("\n");

    // Check if password (or its prefix) is in the pwned list using Bloom filter
    if (bloom_contains(filter, prefix)) 
    {
        printf("Password found in pwned list!\n");
    } 
    else 
    {
        printf("Password not found in pwned list.\n");
    }

    // Free allocated memory
    free_bloom_filter(filter);
    free(password);
    password = NULL;
    
    return 0;
}