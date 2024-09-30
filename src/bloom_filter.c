#include "bloom_filter.h"

BloomFilter* create_bloom_filter(size_t size)
{
    BloomFilter *filter = malloc(sizeof(BloomFilter));
    if (!filter) {
        fprintf(stderr, "Memory allocation failed for BloomFilter\n");
        return NULL;
    }
    filter->bit_array = calloc(size / 8, sizeof(uint8_t));  // Allocate bit array
    filter->size = size;
    filter->count = 0;
    if (!filter->bit_array) {
        fprintf(stderr, "Memory allocation failed for bit_array\n");
        free(filter);
        return NULL;
    }
    return filter;
}

void free_bloom_filter(BloomFilter *filter) 
{
    free(filter->bit_array);
    free(filter);
}

void set_bit(uint8_t *bit_array, uint32_t index) 
{
    bit_array[index / 8] |= (1 << (index % 8));
}

int get_bit(const uint8_t *bit_array, uint32_t index) 
{
    return bit_array[index / 8] & (1 << (index % 8));
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

void bloom_insert(BloomFilter *filter, const char *data) 
{
    set_bit(filter->bit_array, xxhash1(data));
    set_bit(filter->bit_array, xxhash2(data));
    set_bit(filter->bit_array, xxhash3(data));
    filter->count++;
}

int bloom_contains(const BloomFilter *filter, const char *data) 
{
    return get_bit(filter->bit_array, xxhash1(data)) &&
           get_bit(filter->bit_array, xxhash2(data)) &&
           get_bit(filter->bit_array, xxhash3(data));
}

int bloom_filter_is_full(BloomFilter *filter) {
    return filter->count >= (filter->size / 8); // Simple heuristic: full when the bit array is "filled"
}

size_t get_file_size(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        return st.st_size;
    }
    return 0; // Error case
}

int determine_optimal_bloom_filters(size_t file_size) {
    size_t filter_size_bits = BLOOM_SIZE;  // This is the size of one bloom filter in bits (8MB in bits)
    return (file_size + filter_size_bits - 1) / filter_size_bits;  // Number of filters (rounded up)
}

// Populate multiple bloom filters recursively
void populate_recursive_bloom_filters(BloomFilter **filters, FILE *pwned_file, size_t num_filters, size_t current_filter) 
{
    char line[128];
    char hash_prefix[11];
    
    // Read each line from the pwned password file
    while (fgets(line, sizeof(line), pwned_file)) 
    {
        // Assuming each line is a SHA1 hash, copy the first 10 characters (5 bytes in hex)
        strncpy(hash_prefix, line, 10);
        hash_prefix[10] = '\0';  // Null-terminate

        // Insert into the current bloom filter
        bloom_insert(filters[current_filter], hash_prefix);

        // If the current filter is full, move to the next one
        if (bloom_filter_is_full(filters[current_filter])) {
            current_filter = (current_filter + 1) % num_filters;
        }
    }
}

void split_into_bloom_filters(const char *filename, BloomFilter **filters, size_t num_filters) 
{
    FILE *pwned_file = fopen(filename, "r");
    if (!pwned_file) 
    {
        fprintf(stderr, "Could not open file %s.\n", filename);
        return;
    }

    populate_recursive_bloom_filters(filters, pwned_file, num_filters, 0);
    fclose(pwned_file);
}