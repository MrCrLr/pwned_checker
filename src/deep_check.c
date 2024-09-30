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
int deep_check_password(const char *db_path, const char *full_hash) 
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc;
    const char *sql = "SELECT count FROM pwned_passwords WHERE full_hash = ?";

    // Open the SQLite database
    rc = sqlite3_open(db_path, &db);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // Prepare the SQL statement (safe from SQL injection due to prepared statements)
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db); // Ensure the database is closed on failure
        return 1;
    }

    // Bind the full SHA1 hash to the query
    rc = sqlite3_bind_text(stmt, 1, full_hash, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) 
    {
        fprintf(stderr, "Failed to bind hash: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db); // Close database if binding fails
        return 1;
    }

    // Execute the query
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) 
    {
        int count = sqlite3_column_int(stmt, 0);
        printf("Password found in pwned list with %d occurrences!\n", count);
    } 
    else if (rc == SQLITE_DONE) 
    {
        printf("Password not found in pwned list.\n");
    } 
    else 
    {
        fprintf(stderr, "Error in database query: %s\n", sqlite3_errmsg(db));
    }
    printf("Query result: %s\n", rc == SQLITE_ROW ? "Found" : "Not Found"); // DEBUGGER DEBUGGER DEBUGGER

    // Clean up: finalize the statement and close the database
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return 0; // Success
}