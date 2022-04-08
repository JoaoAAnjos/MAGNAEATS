#include "./memory.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "./memory-private.h"

void* create_shared_memory(char* name, int size) {
    // open/create as Read and Write (O_CREAT | O_RDWR) for user (S_IRUSR | S_IWUSR)
    char u_name[NAME_MAX_SIZE] = {0};
    append_uid(name, u_name);

    int fd = shm_open(u_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
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
    memset(ret, 0, size);

    return ret;
}

void* create_dynamic_memory(int size) {
    void* ret = malloc(size);
    if (ret == NULL) {
        printf("Error in malloc");
        exit(-1);
    }
    memset(ret, 0, size);

    return ret;
}

void destroy_shared_memory(char* name, void* ptr, int size) {
    if (munmap(ptr, size) == -1) {
        printf("Error in munmap");
        exit(-1);
    }
    char u_name[NAME_MAX_SIZE] = {0};
    append_uid(name, u_name);
    if (shm_unlink(u_name) == -1) {
        printf("Error in shm_unlink");
        exit(-1);
    }
}

void destroy_dynamic_memory(void* ptr) {
    free(ptr);
}

void write_main_rest_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op) {
    for (int i = 0; i < buffer_size; i++) {
        if (buffer->ptrs[i] == 0) {
            buffer->ptrs[i] = 1;
            copy_operation(&buffer->buffer[i], op);
        }
    }
}

void write_rest_driver_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op) {
    int next = c_next_id(buffer->ptrs->in, buffer_size);

    if (next != buffer->ptrs->out) {
        buffer->ptrs->in = next;
        copy_operation(&buffer->buffer[next], op);
    }
}

void write_driver_client_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op) {
    for (int i = 0; i < buffer_size; i++) {
        if (buffer->ptrs[i] == 0) {
            buffer->ptrs[i] = 1;
            copy_operation(&buffer->buffer[i], op);
        }
    }
}

void read_main_rest_buffer(struct rnd_access_buffer* buffer, int rest_id, int buffer_size, struct operation* op) {
    for (int i = 0; i < buffer_size; i++) {
        if (buffer->ptrs[i] == 1 && buffer->buffer[i].requested_rest == rest_id) {
            copy_operation(op, &buffer->buffer[i]);
            buffer->ptrs[i] = 0;
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
    copy_operation(op, &buffer->buffer[next]);
    buffer->ptrs->out = next;
}

void read_driver_client_buffer(struct rnd_access_buffer* buffer, int client_id, int buffer_size, struct operation* op) {
    for (int i = 0; i < buffer_size; i++) {
        if (buffer->ptrs[i] == 1 && buffer->buffer[i].requesting_client == client_id) {
            copy_operation(op, &buffer->buffer[i]);
            buffer->ptrs[i] = 0;
            return;
        }
    }
    op->id = -1;
}