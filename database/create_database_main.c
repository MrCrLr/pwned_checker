#include "create_database.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <database_path> <pwned_passwords_file>\n", argv[0]);
        return 1;
    }

    const char *db_path = argv[1];
    const char *pwned_file_path = argv[2];

    if (create_pwned_db(db_path, pwned_file_path) == SQLITE_OK) {
        printf("Database created and populated successfully.\n");
    } else {
        printf("Failed to create or populate the database.\n");
    }

    return 0;
}