#include "deep_check.h"

/**
 * Initializes a connection to an SQLite database.
 *
 * This function opens a database file located at the specified path. If the database
 * cannot be opened, an error message is printed, and a non-zero status code is returned.
 *
 * Parameters:
 * - db (sqlite3**): Pointer to the SQLite database connection handle.
 * - db_path (const char*): Path to the SQLite database file.
 *
 * Returns:
 * - int: Returns 0 on successful database connection initialization.
 *   Returns 1 if an error occurs while opening the database.
 *
 * Note:
 * - The caller must ensure that the database connection is eventually closed
 *   to free resources and avoid memory leaks.
 */
int init_db(sqlite3 **db, const char *db_path) {
    int rc = sqlite3_open(db_path, db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
        return 1;
    }
    return 0; // Success
}

/**
 * Performs a deep check to determine if a given hash is present in the database.
 *
 * This function queries the SQLite database to find an exact match for a provided binary hash.
 * It prepares a SQL statement to search for the hash in the 'pwned_passwords' table. If found,
 * it retrieves and prints the count of occurrences, indicating how many times the password
 * associated with this hash has been exposed. If not found, a message stating that the password
 * is not pwned is printed.
 *
 * Parameters:
 * - db (sqlite3*): The SQLite database connection handle.
 * - binary_hash (const char*): The binary hash to be checked, expected to be in raw binary format.
 *
 * Returns:
 * - int: Returns 0 if the function completes successfully, regardless of the hash being found.
 *   Returns -1 if there is an error preparing the SQL statement.
 *
 * Note:
 * - The binary hash should be exactly 20 bytes long as it represents a SHA1 hash.
 * - Ensure that the SQL statement correctly matches the schema of the 'pwned_passwords' table.
 * - This function handles all cleanup of the SQL statement to prevent memory leaks.
 */
int deep_check_password(sqlite3 *db, const unsigned char *binary_hash) 
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