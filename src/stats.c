#include "stats.h"

void writeStats(struct main_data* data, char* fileName, int op_counter) {
    FILE* file = openFile(fileName, "a");

    fprintf(file, "Process Statistics:\n");
    for (int i = 0; i < data->n_restaurants; i++) {
        fprintf(file, "Restaurant %d prepared %d requests!\n", i, *(data->restaurant_stats + i));
    }
    for (int j = 0; j < data->n_drivers; j++) {
        fprintf(file, "Driver %d delivered %d requests!\n", j, *(data->driver_stats + j));
    }
    for (int k = 0; k < data->n_clients; k++) {
        fprintf(file, "Client %d received %d requests!\n", k, *(data->client_stats + k));
    };

    fprintf(file, "\nRequest Statistics\n");
    for(int i = 0; i < op_counter; i++) {
        if((data->results + i)->status == 'C') {
            fprintf(file, "Request: %d\nStatus: %c\nRestaurant id: %d\n"
                    "Driver id:%d\nClient id:%d\n",
                    i,
                    (data->results + i)->status,
                    (data->results + i)->receiving_rest,
                    (data->results + i)->receiving_driver,
                    (data->results + i)->receiving_client);
        }
    }
    closeFile(file);
}

