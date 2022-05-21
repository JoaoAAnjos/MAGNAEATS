#include "synchronization.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

sem_t* semaphore_create(char* name, int value) {
    sem_t* sem = sem_open(name, O_CREAT, 0xFFFFFFFF, value);
    if (sem == SEM_FAILED) {
        printf("Error in sem_open");
        exit(-1);
    }

    return sem;
}

void semaphore_destroy(char* name, sem_t* semaphore) {
    if (sem_close(semaphore) == -1) {
        printf("error in sem_close");
        exit(-1);
    }
    if (sem_unlink(name) == -1) {
        printf("error in sem_unlink");
        exit(-1);
    }
}

void produce_begin(struct prodcons* pc) {
    semaphore_mutex_lock(pc->empty);
    semaphore_mutex_lock(pc->mutex);
}

void produce_end(struct prodcons* pc) {
    semaphore_mutex_unlock(pc->mutex);
    semaphore_mutex_unlock(pc->full);
}

void consume_begin(struct prodcons* pc) {
    semaphore_mutex_lock(pc->full);
    semaphore_mutex_lock(pc->mutex);
}

void consume_end(struct prodcons* pc) {
    semaphore_mutex_unlock(pc->mutex);
    semaphore_mutex_unlock(pc->empty);
}

void semaphore_mutex_lock(sem_t* sem) {
    if (sem_wait(sem) == -1) {
        printf("error in sem_wait");
        exit(-1);
    }
}

void semaphore_mutex_unlock(sem_t* sem) {
    if (sem_post(sem) == -1) {
        printf("error in sem_post");
        exit(-1);
    }
}