#include "memory-private.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int c_next_id(int id, int size) {
    int next = (id + 1) % size;
    return next ? next : 1;
}

void append_uid(char* name, char* dest) {
    if (snprintf(dest, NAME_MAX_SIZE, "%s-%i", name, getuid()) < 0) {
        printf("Error in append_uid");
        exit(-1);
    }
}

void copy_operation(struct operation* dest, struct operation* src) {
    dest->id = src->id;
    dest->requested_rest = src->requested_rest;
    dest->requesting_client = src->requesting_client;
    dest->requested_dish = src->requested_dish;
    dest->status = src->status;
    dest->receiving_rest = src->receiving_rest;
    dest->receiving_driver = src->receiving_driver;
    dest->receiving_client = src->receiving_client;
}