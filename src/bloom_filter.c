#include "bloom_filter.h"

BloomFilter* create_bloom_filter(size_t size)
{
    BloomFilter *filter = malloc(sizeof(BloomFilter));
    filter->bit_array = calloc(BLOOM_SIZE / 8, sizeof(uint8_t));  // Allocate bit array
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
}

int bloom_contains(const BloomFilter *filter, const char *data) 
{
    return get_bit(filter->bit_array, xxhash1(data)) &&
           get_bit(filter->bit_array, xxhash2(data)) &&
           get_bit(filter->bit_array, xxhash3(data));
}

void populate_bloom_filter(BloomFilter *filter, FILE *pwned_file) 
{
    char line[128]; // Assuming each line has enough space for SHA1 + other data
    char hash_prefix[11]; // To hold the first 5 bytes (10 characters in hex) of the pwned password hashes

    // Read each line from the pwned password file
    while (fgets(line, sizeof(line), pwned_file)) 
    {
        // Assuming that the pwned password file contains SHA1 hashes
        strncpy(hash_prefix, line, 10); // Copy the first 10 characters (5 bytes in hex) of the hash
        hash_prefix[10] = '\0';         // Null-terminate the string

        // Insert the hash prefix into the Bloom filter
        bloom_insert(filter, hash_prefix);
    }
}


/*
void populate_bloom_filter(BloomFilter *filter, const void *pwned_data, size_t file_size) 
{
    const char *line_start = (const char*) pwned_data;
    const char *file_end = line_start + file_size;

    while (line_start < file_end) {
        const char *line_end = memchr(line_start, '\n', file_end - line_start);
        if (!line_end) {
            break;  // No more lines found
        }

        // Insert the line into the Bloom filter (here you are assuming each line is a hash)
        bloom_insert(filter, line_start);

        line_start = line_end + 1;  // Move to next line
    }
}

void populate_bloom_filter(BloomFilter *filter, const char *filename) 
{
    // Open the file using open() instead of fopen()
    int fd = open(filename, O_RDONLY);
    if (fd == -1) 
    {
        perror("open");
        return;
    }

    // Get the file size
    struct stat sb;
    if (fstat(fd, &sb) == -1) 
    {
        perror("fstat");
        close(fd);
        return;
    }

    // Memory-map the file
    char *file_data = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_data == MAP_FAILED) 
    {
        perror("mmap");
        close(fd);
        return;
    }

    // Now that the file is memory-mapped, process it like a large array
    char *line_start = file_data;
    char *line_end = NULL;

    // Iterate over the entire file
    while (line_start < file_data + sb.st_size) 
    {
        // Find the next newline character
        line_end = memchr(line_start, '\n', file_data + sb.st_size - line_start);
        if (!line_end) 
        {
            break;  // No more lines
        }

        // Null-terminate the current line
        *line_end = '\0';

        // Insert the line into the Bloom filter
        bloom_insert(filter, line_start);

        // Move to the next line
        line_start = line_end + 1;
    }

    // Unmap the memory and close the file
    munmap(file_data, sb.st_size);
    close(fd);
}




#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include "bloom_filter.h"

void populate_bloom_filter(BloomFilter *filter, const char *filename) 
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1) 
    {
        perror("open");
        return;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) 
    {
        perror("fstat");
        close(fd);
        return;
    }

    char *file_contents = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_contents == MAP_FAILED) 
    {
        perror("mmap");
        close(fd);
        return;
    }

    close(fd);

    char *line = strtok(file_contents, "\n");
    while (line != NULL) 
    {
        bloom_insert(filter, line); // Insert each line into the Bloom filter
        line = strtok(NULL, "\n");
    }

    munmap(file_contents, sb.st_size); // Unmap the file after processing
}
*/