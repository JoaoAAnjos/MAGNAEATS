#include "stats.h"

void writeStats(struct main_data* data, char* fileName, int op_counter) {
    FILE* file = openFile(fileName, "a");
    char buffer[CHAR_BUFFER_SIZE];

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
            strftime(buffer, CHAR_BUFFER_SIZE, "%Y-%m-%d %H:%M:%S\n", localtime(&(data->results + i)->start_time.tv_sec));
            fprintf(file, "Created: %s", buffer);
            memset(&buffer, 0, CHAR_BUFFER_SIZE);
            strftime(buffer, CHAR_BUFFER_SIZE, "%Y-%m-%d %H:%M:%S\n", localtime(&(data->results + i)->rest_time.tv_sec));
            fprintf(file, "Restaurant time: %s", buffer);
            memset(&buffer, 0, CHAR_BUFFER_SIZE);
            strftime(buffer, CHAR_BUFFER_SIZE, "%Y-%m-%d %H:%M:%S\n", localtime(&(data->results + i)->driver_time.tv_sec));
            fprintf(file, "Driver time: %s", buffer);
            memset(&buffer, 0, CHAR_BUFFER_SIZE);
            strftime(buffer, CHAR_BUFFER_SIZE, "%Y-%m-%d %H:%M:%S\n", localtime(&(data->results + i)->client_end_time.tv_sec));
            fprintf(file, "Client time (end): %s", buffer);
            fprintf(file, "Total time: %s", buffer);
        }
    }
    closeFile(file);
}

