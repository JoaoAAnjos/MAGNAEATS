#include "mesignal.h"

struct main_data* pdata;
struct communication_buffers* pbuffers;
struct semaphores* psems;
int* p_op_counter;


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
    printf("\nStop signal caught\n\n");
    stop_execution(pdata, pbuffers, psems);
    exit(0);
}

void set_alarm(int alarm_time, int* op_counter) {
    p_op_counter = op_counter;
    struct itimerval val;
    signal(SIGALRM, handle_alarm);
    val.it_interval.tv_sec = alarm_time;
    val.it_interval.tv_usec = 0;
    val.it_value.tv_sec = alarm_time;
    val.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &val, 0);
}

void handle_alarm() {
    if(!*pdata->terminate) {
        semaphore_mutex_lock(psems->results_mutex);
        for(int i = 0; i < *p_op_counter; i++) {
            if((pdata->results + i)->status == 'C') {
                printf("request:%d status:%c start:%ld restaurant:%d rest_time:%ld"
                        " driver:%d driver_time:%ld client:%d client_end_time:%ld\n",
                        i,
                        (pdata->results + i)->status,
                        (pdata->results + i)->start_time.tv_sec,
                        (pdata->results + i)->receiving_rest,
                        (pdata->results + i)->rest_time.tv_sec,
                        (pdata->results + i)->receiving_driver,
                        (pdata->results + i)->driver_time.tv_sec,
                        (pdata->results + i)->receiving_client,
                        (pdata->results + i)->client_end_time.tv_sec
                        );
            } else {
                printf("request:%d status:%c\n", i, (pdata->results + i)->status);
            }
        }
        semaphore_mutex_unlock(psems->results_mutex);
        signal(SIGALRM, handle_alarm);
    }

}