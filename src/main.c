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
        char *password = get_password_input(maxPasswordLength);
        if (password == NULL) {
            fprintf(stderr, "Failed to get password input. Please try again.\n");
            continue;
        }

        // Compute the SHA-1 hash of the password
        unsigned char hash[SHA_DIGEST_LENGTH];
        SHA1((unsigned char*)password, strlen(password), hash);
        
        // Perform a deep check using the binary hash directly
        if (deep_check_password(db, (char *)hash) != 0) {
            fprintf(stderr, "Error during the deep check.\n");
        } else {
            printf("Password check completed successfully.\n");
        }

        free(password);
        answer = get_yes_no_response();

    } while (answer == 'y');

    sqlite3_close(db);
    return 0;
}