#include "memory-private.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int c_next_id(int id, int size) {
    return ((id + 1) % size);
}

void append_uid(char* name, char* dest) {
    if (snprintf(dest, NAME_MAX_SIZE, "%s-%i", name, getuid()) < 0) {
        printf("Error in append_uid");
        exit(-1);
    }
}