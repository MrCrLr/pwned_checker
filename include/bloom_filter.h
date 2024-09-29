#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <xxhash.h>

#define BLOOM_SIZE (1 << 24)  // 16 million bits (2MB)
#define NUM_HASHES 3          // Number of hash functions

// BloomFilter structure definition
typedef struct {
    uint8_t *bit_array;  // Pointer to the bit array
    size_t size;         // Size of the bit array in bits
} BloomFilter;

// Function prototypes for working with the Bloom filter
BloomFilter* create_bloom_filter(size_t size); // Create and initialize a Bloom filter
void free_bloom_filter(BloomFilter *filter);   // Free memory allocated for Bloom filter
void set_bit(uint8_t *bit_array, uint32_t index);
int get_bit(const uint8_t *bit_array, uint32_t index);
void bloom_insert(BloomFilter *filter, const char *data);
int bloom_contains(const BloomFilter *filter, const char *data);
void populate_bloom_filter(BloomFilter *filter, FILE *pwned_file);

// Hash functions (can be improved for better distribution)
uint32_t xxhash1(const char *data);
uint32_t xxhash2(const char *data);
uint32_t xxhash3(const char *data);

#endif // BLOOM_FILTER_H