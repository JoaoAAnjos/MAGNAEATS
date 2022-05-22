#ifndef MESIGNAL_H_GUARD
#define MESIGNAL_H_GUARD

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include "main.h"
#include "synchronization.h"

/*
*/
void setup_stop(struct main_data* data, struct communication_buffers* buffers, struct semaphores* sems);

/*
*/
void handle_sigint(int sig);

/*
*/
void set_alarm(int alarm_time, int* op_counter);

/*
*/
void handle_alarm();


#endif