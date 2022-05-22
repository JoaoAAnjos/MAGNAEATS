#include "process.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "client.h"
#include "driver.h"
#include "restaurant.h"

int launch_restaurant(int restaurant_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    int pid = fork();
    if (pid == -1) {
        printf("Error forking in launch_restaurant");
        exit(-1);
    } else if (pid == 0) {
        exit(execute_restaurant(restaurant_id, buffers, data, sems));
    }
    return pid;
}

int launch_driver(int driver_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    int pid = fork();
    if (pid == -1) {
        printf("Error forking in launch_restaurant");
        exit(-1);
    } else if (pid == 0) {
        exit(execute_driver(driver_id, buffers, data, sems));
    }
    return pid;
}

int launch_client(int client_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    int pid = fork();
    if (pid == -1) {
        printf("Error forking in launch_restaurant");
        exit(-1);
    } else if (pid == 0) {
        exit(execute_client(client_id, buffers, data, sems));
    }
    return pid;
}

int wait_process(int process_id) {
    int status;
    waitpid(process_id, &status, 0);
    if (!WIFEXITED(status)) {
        if (WEXITSTATUS(status) == 0) {
            return WEXITSTATUS(status);
        }
        
        printf("Error waiting for processes: %d\n", WEXITSTATUS(status));
        exit(-1);
    }
    return WEXITSTATUS(status);
}