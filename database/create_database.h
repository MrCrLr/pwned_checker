#ifndef CREATE_DATABASE_H
#define CREATE_DATABASE_H

#include <stdio.h>
#include <sqlite3.h>
#include <string.h>

int create_pwned_db(const char *db_path, const char *pwned_file_path);

#endif // CREATE_DATABASE_H