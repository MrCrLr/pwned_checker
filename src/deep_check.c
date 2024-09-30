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
#include "deep_check.h"

// Function to check for an exact match in the SQLite DB using the prefix and then the full hash
int deep_check_password(const char *db_path, const char *hash_prefix, const char *hash) 
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc;
    char sql[256];

    // Construct SQL query using the prefix to limit the search
    snprintf(sql, sizeof(sql), "SELECT full_hash, count FROM pwned_passwords WHERE full_hash LIKE '%s%%'", hash_prefix);

    // Open the SQLite database
    rc = sqlite3_open(db_path, &db);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // Prepare the SQL statement
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    // Execute the query and handle results
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) 
    {
        const char *db_full_hash = (const char *)sqlite3_column_text(stmt, 0);
        int count = sqlite3_column_int(stmt, 1);

        // Verify the full hash
        if (strcmp(hash, db_full_hash) == 0) 
        {
            printf("Password found: %s, occurrences: %d\n", db_full_hash, count);
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return 0; // Found a match
        }
    }

    if (rc != SQLITE_DONE) 
    {
        fprintf(stderr, "Error in database query: %s\n", sqlite3_errmsg(db));
    }

    // Clean up
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    // If no match was found
    printf("Password not found in pwned list.\n");
    return 0;
}