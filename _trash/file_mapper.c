#include "file_mapper.h"

// Function to map the pwned password file using mmap
void* map_file(const char *filename, size_t *file_size) {
    int fd = open(filename, O_RDONLY);  // Open file descriptor
    if (fd == -1) {
        perror("open");
        return NULL;
    }

    struct stat file_stat;
    if (fstat(fd, &file_stat) == -1) {
        perror("fstat");
        close(fd);
        return NULL;
    }

    *file_size = file_stat.st_size;

    // Memory-map the file
    void *file_content = mmap(NULL, *file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_content == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return NULL;
    }

    close(fd);  // Close file descriptor
    return file_content;  // Return mapped content
}

// Function to unmap the file after usage
void unmap_file(void *file_content, size_t file_size) {
    if (munmap(file_content, file_size) == -1) {
        perror("munmap");
    }
}