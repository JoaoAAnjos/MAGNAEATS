#include "./memory.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define S_OP sizeof(struct operation*)

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
    for (int i = 0; i < buffer_size; i++) {
        if (buffer->ptrs[i * sizeof(int*)] == 0) {
            buffer->ptrs[i * sizeof(int*)] = 1;
            buffer->buffer[i * S_OP] = *op;
        }
    }
}

void write_rest_driver_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op) {
    int next = c_next_id(buffer->ptrs->in, buffer_size);

    if (next != buffer->ptrs->out) {
        buffer->ptrs->in = next;
        buffer->buffer[next * S_OP] = *op;
    }
}

void write_driver_client_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op) {
    for (int i = 0; i < buffer_size; i++) {
        if (buffer->ptrs[i * sizeof(int*)] == 0) {
            buffer->buffer[i * S_OP] = *op;
        }
    }
}

void read_main_rest_buffer(struct rnd_access_buffer* buffer, int rest_id, int buffer_size, struct operation* op) {
    for (int i = 0; i < buffer_size; i++) {
        // TODO: understand if it is requested_rest or receiving_rest
        if (buffer->ptrs[i * sizeof(int*)] == 1 && buffer->buffer[i * S_OP].requested_rest == rest_id) {
            // TODO: Copy operation or post in forum
            return;
        }
    }
    op->id = -1;
}

void read_rest_driver_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op) {
    int next = c_next_id(buffer->ptrs->out, buffer_size);

    if (next == c_next_id(buffer->ptrs->in, buffer_size)) {
        op->id = -1;
        return;
    }
    // TODO: Copy operation or post in forum
}

void read_driver_client_buffer(struct rnd_access_buffer* buffer, int client_id, int buffer_size, struct operation* op) {
    for (int i = 0; i < buffer_size; i++) {
        // TODO: understand if it is requesting_client or receiving_client
        if (buffer->ptrs[i * sizeof(int*)] == 1 && buffer->buffer[i * S_OP].receiving_client == client_id) {
            // TODO: Copy operation or post in forum
            return;
        }
    }
    op->id = -1;
}

/* Funtion to calculate the next id for a circular buffer. Based on a given id and a given size
 */
int c_next_id(int id, int size) {
    int next = (id + 1) % size;
    return next ? next : 1;
}