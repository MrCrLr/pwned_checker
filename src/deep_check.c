#include "deep_check.h"

// Function to initialize the SQLite DB
int init_db(sqlite3 **db, const char *db_path) {
    int rc = sqlite3_open(db_path, db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
        return 1;
    }
    return 0; // Success
}

// Function to check for an exact match in the SQLite DB using full hash
int deep_check_password(sqlite3 *db, const char *binary_hash) 
{
    sqlite3_stmt *stmt;
    int rc;
    const char *sql = "SELECT count FROM pwned_passwords WHERE full_hash = ?";

    // Prepare the SQL statement
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    // Bind the binary hash to the query using BLOB
    sqlite3_bind_blob(stmt, 1, binary_hash, 20, SQLITE_STATIC);

    // Execute the query
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) 
    {
        int count = sqlite3_column_int(stmt, 0);
        printf("Password found in pwned list with %d occurrences!\n", count);
    } 
    else 
    {
        printf("Password not found in pwned list.\n");
    }

    // Clean up
    sqlite3_finalize(stmt);
    return 0; // Success
}