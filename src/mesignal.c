#include "mesignal.h"

struct main_data* pdata;
struct communication_buffers* pbuffers;
struct semaphores* psems;


void setup_stop(struct main_data* data, struct communication_buffers* buffers, struct semaphores* sems) {
    pdata = data;
    pbuffers = buffers;
    psems = sems;
    struct sigaction sa;
    sa.sa_handler = &handle_sigint;
    sa.sa_flags = 0;
    sigaddset(&sa.sa_mask, SIGINT);
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        printf("Error in sigaction");
        exit(-1);
    }
}

void handle_sigint(int sig) {
    printf("Stop signal caught\n");
    stop_execution(pdata, pbuffers, psems);
    exit(0);
}