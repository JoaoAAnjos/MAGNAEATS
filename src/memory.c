#include "./memory.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

void* create_shared_memory(char* name, int size) {
    // open as Read and Write (O_RDWR) for user (S_IRUSR | S_IWUSR)
    int fd = shm_open(name, O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        printf("Error in shm_open");
        exit(-1);
    }
    // Define size
    if (ftruncate(fd, size) == -1) {
        printf("Error in ftruncate");
        exit(-1);
    }
    // Project file abstraction. Permission read/write (PROT_READ | PROT_WRITE). Shared flag (MAP_SHARED)
    void* ret = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (ret == MAP_FAILED) {
        printf("Error in mmap");
        exit(-1);
    }

    return ret;
}

void* create_dynamic_memory(int size) {
    void* ret = malloc(size);
    if (ret == NULL) {
        printf("Error in malloc");
        exit(-1);
    }
    return ret;
}

void destroy_shared_memory(char* name, void* ptr, int size) {
    if (munmap(ptr, size) == -1) {
        printf("Error in munmap");
        exit(-1);
    }
    if (shm_unlink(name) == -1) {
        printf("Error in shm_unlink");
        exit(-1);
    }
}

void destroy_dynamic_memory(void* ptr) {
    free(ptr);
}

void write_main_rest_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op) {
    for (size_t i = 0; i < buffer_size; i++) {
        if (buffer->ptrs[i * sizeof(int*)]) {
            buffer->buffer[i * sizeof(struct operation*)] = *op;
        }
    }
}
