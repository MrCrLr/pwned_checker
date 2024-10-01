#ifndef CREATE_DATABASE_H
#define CREATE_DATABASE_H

#include <stdio.h>
#include <sqlite3.h>
#include <string.h>

int create_pwned_db(const char *db_path, const char *pwned_file_path);

// Utility function to convert a hex string to a binary array
void hex_to_bin_sql(const char *hex, unsigned char *bin);

#endif // CREATE_DATABASE_H