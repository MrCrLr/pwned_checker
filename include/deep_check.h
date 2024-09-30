#ifndef DEEP_CHECK_H
#define DEEP_CHECK_H

#include <openssl/sha.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to perform a deep check using full SHA1 hash
int deep_check_password(const char *db_path, const char *hash_prefix, const char *hash);

// Function to initialize the SQLite database (if needed)
int init_db(sqlite3 **db, const char *db_path);

#endif // DEEP_CHECK_H