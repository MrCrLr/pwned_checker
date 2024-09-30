#include "create_database.h"

// Function to create SQLite DB and load data from the pwned passwords file
int create_pwned_db(const char *db_path, const char *pwned_file_path) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char *err_msg = 0;

    // Open the database
    int rc = sqlite3_open(db_path, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return rc;
    }

    // Create the pwned passwords table if it doesn't exist
    const char *sql_create_table = "CREATE TABLE IF NOT EXISTS pwned_passwords("
                                   "full_hash TEXT PRIMARY KEY,"
                                   "count INTEGER);";
                                   
    rc = sqlite3_exec(db, sql_create_table, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return rc;
    }

    // Create an index on the full_hash column for faster lookups
    const char *sql_create_index = "CREATE INDEX IF NOT EXISTS idx_full_hash ON pwned_passwords(full_hash);";
    
    rc = sqlite3_exec(db, sql_create_index, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error (creating index): %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return rc;
    }

    // Prepare the SQL insert statement
    const char *sql_insert = "INSERT OR IGNORE INTO pwned_passwords(full_hash, count) VALUES(?, ?)";
    rc = sqlite3_prepare_v2(db, sql_insert, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return rc;
    }

    // Begin transaction for faster inserts
    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to begin transaction: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return rc;
    }

    // Open the pwned passwords file
    FILE *pwned_file = fopen(pwned_file_path, "r");
    if (!pwned_file) {
        fprintf(stderr, "Could not open pwned password file: %s\n", pwned_file_path);
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    }

    // Read the file and insert into SQLite
    char line[128];
    char hash[41]; // 40 characters for SHA1 hash + 1 for null-terminator
    int count;
    while (fgets(line, sizeof(line), pwned_file)) {
        sscanf(line, "%40[^:]:%d", hash, &count); // Parse the hash and count

        // Bind values to the SQL statement
        sqlite3_bind_text(stmt, 1, hash, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, count);

        // Execute the SQL statement
        sqlite3_step(stmt);
        sqlite3_reset(stmt);  // Reset the statement to reuse it
    }

    // Commit transaction
    rc = sqlite3_exec(db, "COMMIT;", NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to commit transaction: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    fclose(pwned_file);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    return SQLITE_OK;
}