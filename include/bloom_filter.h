#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include <fcntl.h>
#include <sqlite3.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <xxhash.h>

#define BLOOM_SIZE (8 * 1024 * 1024 * 8)  // 8MB in bits = 67,108,864 bits
#define NUM_HASHES 3                      // Number of hash functions

// BloomFilter structure definition
typedef struct {
    uint8_t *bit_array;  // Pointer to the bit array
    size_t size;         // Size of the bit array in bits
    size_t count;        // Number of inserted elements (for tracking usage)
} BloomFilter;

// Function prototypes for working with the Bloom filter
BloomFilter* create_bloom_filter(size_t size); // Create and initialize a Bloom filter
void free_bloom_filter(BloomFilter *filter);   // Free memory allocated for Bloom filter
void set_bit(uint8_t *bit_array, uint32_t index);
int get_bit(const uint8_t *bit_array, uint32_t index);
void bloom_insert(BloomFilter *filter, const char *data);
int bloom_contains(const BloomFilter *filter, const char *data);

// Determine size of dataset and decide number of bloom filters necessary
int get_password_count(sqlite3 *db);
int determine_optimal_bloom_filters(size_t file_size);

// Recursively populate multiple Bloom filters
void populate_bloom_filters_from_db(BloomFilter **filters, size_t num_filters, sqlite3 *db);
void split_into_bloom_filters(const char *filename, BloomFilter **filters, size_t num_filters);

// Check if a Bloom filter is "full"
int bloom_filter_is_full(BloomFilter *filter);

// Hash functions (using xxhash for performance)
uint32_t xxhash1(const char *data);
uint32_t xxhash2(const char *data);
uint32_t xxhash3(const char *data);

#endif // BLOOM_FILTER_H