#include "deep_check.h"
#include "program.h"

int main() {
    // Initialize SQLite for deep check
    sqlite3 *db;
    if (init_db(&db, "database/pwnedpasswords.db") != 0) {
        fprintf(stderr, "Failed to initialize the database.\n");
        return 1;
    }

    atexit(cleanup);
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    char answer;
    const int maxPasswordLength = 100;

    do {
        SecureBuffer securePassword = get_password_input(maxPasswordLength);
        if (securePassword.buffer == NULL || securePassword.size == 0) {
            fprintf(stderr, "Failed to get password input. Please try again.\n");
            continue;
        }

        // Compute the SHA-1 hash of the password
        unsigned char hash[SHA_DIGEST_LENGTH];
        SHA1((unsigned char*)securePassword.buffer, strlen(securePassword.buffer), hash);
        
        // Perform a deep check using the binary hash directly
        if (deep_check_password(db, (char *)hash) != 0) {
            fprintf(stderr, "Error during the deep check.\n");
        } else {
            printf("Check complete.\n\n");
        }

        secure_free(securePassword.buffer, securePassword.size); // Securely free the password
        securePassword.buffer = NULL; // Ensure pointer is cleared
        answer = get_yes_no_response();

    } while (answer == 'y');
    
    printf("Always use strong passwords. Goodbye!\n\n");

    sqlite3_close(db);
    return 0;
}