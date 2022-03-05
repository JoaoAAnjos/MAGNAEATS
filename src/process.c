#include "process.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "client.h"
#include "driver.h"
#include "restaurant.h"

int launch_restaurant(int restaurant_id, struct communication_buffers* buffers, struct main_data* data) {
    int pid = fork();
    if (pid == -1) {
        printf("Error forking in launch_restaurant");
        exit(-1);
    } else if (pid == 0) {
        // TODO: execute_restaurant()
    }
    return pid;
}

int launch_driver(int driver_id, struct communication_buffers* buffers, struct main_data* data) {
    int pid = fork();
    if (pid == -1) {
        printf("Error forking in launch_restaurant");
        exit(-1);
    } else if (pid == 0) {
        // TODO: execute_driver()
    }
    return pid;
}

int launch_client(int client_id, struct communication_buffers* buffers, struct main_data* data) {
    int pid = fork();
    if (pid == -1) {
        printf("Error forking in launch_restaurant");
        exit(-1);
    } else if (pid == 0) {
        // TODO: execute_client()
    }
    return pid;
}

int wait_process(int process_id) {
    int status;
    waitpid(process_id, &status, 0);
    if (!WIFEXITED(status)) {
        printf("Error in wait_process");
        exit(-1);
    }
    return WEXITSTATUS(status);
}