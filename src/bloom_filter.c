#include "bloom_filter.h"

/**
 * Gets the size of a file.
 * 
 * @param filename: The name of the file.
 * @return: The size of the file in bytes, or 0 if an error occurs.
 
size_t get_file_size(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        return st.st_size;
    }
    return 0; // Error case
}
*/
int get_password_count(sqlite3 *db) {
    const char *sql = "SELECT COUNT(*) FROM pwned_passwords;";
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return -1; // Error case
    }

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count;
}

/**
 * Determines the optimal number of Bloom filters needed based on the file size.
 * 
 * @param file_size: The size of the file in bytes.
 * @return: The optimal number of Bloom filters.
 */
int determine_optimal_bloom_filters(size_t file_size) {
    size_t filter_size_bits = BLOOM_SIZE; // Size of one bloom filter in bits (8MB in bits)
    return (file_size + filter_size_bits - 1) / filter_size_bits; // Number of filters (rounded up)
}

/**
 * Creates and initializes a Bloom filter.
 * 
 * @param size: The size of the Bloom filter in bits.
 * @return: A pointer to the created Bloom filter.
 */
BloomFilter* create_bloom_filter(size_t size) {
    BloomFilter *filter = malloc(sizeof(BloomFilter));
    if (!filter) {
        fprintf(stderr, "Memory allocation failed for BloomFilter\n");
        return NULL;
    }
    filter->bit_array = calloc(size / 8, sizeof(uint8_t)); // Allocate bit array
    filter->size = size;
    filter->count = 0;
    if (!filter->bit_array) {
        fprintf(stderr, "Memory allocation failed for bit_array\n");
        free(filter);
        return NULL;
    }
    return filter;
}

/**
 * Splits the pwned passwords file into multiple Bloom filters.
 * 
 * @param filename: The name of the pwned password file.
 * @param filters: An array of Bloom filters.
 * @param num_filters: The number of Bloom filters.

void split_into_bloom_filters(const char *filename, BloomFilter **filters, size_t num_filters) {
    FILE *pwned_file = fopen(filename, "r");
    if (!pwned_file) {
        fprintf(stderr, "Could not open file %s.\n", filename);
        return;
    }

    populate_recursive_bloom_filters(filters, pwned_file, num_filters, 0);
    fclose(pwned_file);
}
 */

/**
 * Populates multiple Bloom filters recursively from the pwned password file.
 * 
 * @param filters: An array of Bloom filters.
 * @param pwned_file: The file containing pwned passwords.
 * @param num_filters: The number of Bloom filters.
 * @param current_filter: The index of the current Bloom filter being populated.
 * 
void populate_recursive_bloom_filters(BloomFilter **filters, FILE *pwned_file, size_t num_filters, size_t current_filter) {
    char line[128];
    char hash_prefix[11];
    
    // Read each line from the pwned password file
    while (fgets(line, sizeof(line), pwned_file)) {
        strncpy(hash_prefix, line, 10); // Copy the first 10 characters (5 bytes in hex)
        hash_prefix[10] = '\0'; // Null-terminate

        // Insert into the current Bloom filter
        bloom_insert(filters[current_filter], hash_prefix);

        // If the current filter is full, move to the next one
        if (bloom_filter_is_full(filters[current_filter])) {
            current_filter = (current_filter + 1) % num_filters;
        }
    }
}
*/

void populate_bloom_filters_from_db(BloomFilter **filters, size_t num_filters, sqlite3 *db) {
    const char *sql = "SELECT full_hash FROM pwned_passwords LIMIT ? OFFSET ?";
    sqlite3_stmt *stmt;

    // Prepare the SQL statement
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    size_t batch_size = 10000; // Number of entries to fetch in each batch
    size_t offset = 0;
    int filter_index = 0;

    while (1) {
        // Bind the batch size and offset
        sqlite3_bind_int(stmt, 1, batch_size);
        sqlite3_bind_int(stmt, 2, offset);

        // Execute the statement
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_DONE) {
            break; // No more rows to fetch
        } else if (rc != SQLITE_ROW) {
            fprintf(stderr, "Error during fetching: %s\n", sqlite3_errmsg(db));
            break;
        }

        // Process the fetched rows
        for (size_t i = 0; i < batch_size; i++) {
            const char *hash = (const char *)sqlite3_column_text(stmt, 0);

            // Insert the hash into the Bloom filter
            bloom_insert(filters[filter_index], hash);

            // If the current filter is full, move to the next one
            if (bloom_filter_is_full(filters[filter_index])) {
                filter_index = (filter_index + 1) % num_filters;
            }

            // Check if there are more rows to process
            if (sqlite3_step(stmt) != SQLITE_ROW) {
                break; // Exit if there are no more rows
            }
        }
        offset += batch_size; // Update the offset for the next batch
    }

    sqlite3_finalize(stmt); // Clean up the prepared statement
}

/**
 * Inserts data into the Bloom filter.
 * 
 * @param filter: The Bloom filter to insert into.
 * @param data: The data to insert.
 */
void bloom_insert(BloomFilter *filter, const char *data) {
    set_bit(filter->bit_array, xxhash1(data));
    set_bit(filter->bit_array, xxhash2(data));
    set_bit(filter->bit_array, xxhash3(data));
    filter->count++;
}

/**
 * Sets a bit in the Bloom filter's bit array.
 * 
 * @param bit_array: The bit array.
 * @param index: The index of the bit to set.
 */
void set_bit(uint8_t *bit_array, uint32_t index) {
    bit_array[index / 8] |= (1 << (index % 8));
}

// From the xxHash library
uint32_t xxhash1(const char *data) {
    return XXH32(data, strlen(data), 0) % BLOOM_SIZE;
}

uint32_t xxhash2(const char *data) {
    return XXH32(data, strlen(data), 1) % BLOOM_SIZE;
}

uint32_t xxhash3(const char *data) {
    return XXH32(data, strlen(data), 2) % BLOOM_SIZE;
}

/**
 * Checks if the Bloom filter is full.
 * 
 * @param filter: The Bloom filter to check.
 * @return: 1 if full, 0 otherwise.
 */
int bloom_filter_is_full(BloomFilter *filter) {
    return filter->count >= (filter->size / 8); // Simple heuristic: full when the bit array is "filled"
}

/**
 * Checks if the Bloom filter contains the specified data.
 * 
 * @param filter: The Bloom filter to check.
 * @param data: The data to check for.
 * @return: 1 if the data is possibly in the filter, 0 otherwise.
 */
int bloom_contains(const BloomFilter *filter, const char *data) {
    return get_bit(filter->bit_array, xxhash1(data)) &&
           get_bit(filter->bit_array, xxhash2(data)) &&
           get_bit(filter->bit_array, xxhash3(data));
}

/**
 * Gets the value of a specific bit in the Bloom filter's bit array.
 * 
 * @param bit_array: The bit array.
 * @param index: The index of the bit to get.
 * @return: The value of the bit (0 or 1).
 */
int get_bit(const uint8_t *bit_array, uint32_t index) {
    return bit_array[index / 8] & (1 << (index % 8));
}

/**
 * Frees the memory allocated for a Bloom filter.
 * 
 * @param filter: The Bloom filter to free.
 */
void free_bloom_filter(BloomFilter *filter) {
    free(filter->bit_array);
    free(filter);
}