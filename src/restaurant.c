#include "restaurant.h"
#include <stdio.h>

int counter = 0;

int execute_restaurant(int rest_id, struct communication_buffers* buffers, struct main_data* data){
    while(1) {
        struct operation* op = NULL;
        restaurant_receive_operation(op, rest_id, buffers, data);
        if(op != NULL && *data->terminate == 0) {
            restaurant_process_operation(op, rest_id, data, &counter);
            restaurant_forward_operation(op, buffers, data);
        } else if (*data->terminate == 1) {
            return counter;
        }
    }
}

void restaurant_receive_operation(struct operation* op, int rest_id, struct communication_buffers* buffers, struct main_data* data){
    if(*data->terminate != 1) {
        
    } else {
        return;
    }
}

void restaurant_process_operation(struct operation* op, int rest_id, struct main_data* data, int* counter) {
    op->receiving_rest = rest_id;
    op->status = 'R';
    *counter += 1;
    *data->restaurant_stats += 1;
}