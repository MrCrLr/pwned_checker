#ifndef FILE_MAPPER_H
#define FILE_MAPPER_H

#include <fcntl.h>
#include <stddef.h>  // For size_t
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

// Function prototypes for mapping and unmapping files
void* map_file(const char *filename, size_t *file_size);
void unmap_file(void *file_content, size_t file_size);

#endif