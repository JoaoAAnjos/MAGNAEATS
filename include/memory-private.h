#ifndef MEMORY_H_GUARD_PRIVATE
#define MEMORY_H_GUARD_PRIVATE

#define NAME_MAX_SIZE 50

#include "memory.h"

/* Funtion to calculate the next id for a circular buffer. Based on a given id and a given size
 */
int c_next_id(int id, int size);

/* Function to append uid to given name. Name + uid can only be up to 49 chars
 */
void append_uid(char* name, char* dest);

/* Copies operation contents to another operation
 */
void copy_operation(struct operation* dest, struct operation* src);

#endif