#include "main.h"
#include "process.h"
#include "configuration.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REQUEST "request"
#define STATUS "status"
#define STOP "stop"
#define HELP "help"
#define COMMANDS \
    "The following commands are possible:\n\n\
 %s - creates a new operation\n\
    usage: request [client_id] [restaurant_id] [the_dish]\n\
 %s - displays the status of an operation\n\
    usage: status [operation_number]\n\
 %s - stops the execution of this program, all its child processes\
    and prints statistics\n\
 %s - prints this message\n"

#define COMMAND_MAX_SIZE 8
#define CHAR_BUFFER_SIZE 100

char* log_filename;
char* statistics_filename;
int* alarm_time;

/* Função que lê os argumentos da aplicação, nomeadamente o número
 * máximo de operações, o tamanho dos buffers de memória partilhada
 * usados para comunicação, e o número de clientes, de motoristas e de
 * restaurantes. Guarda esta informação nos campos apropriados da
 * estrutura main_data.
 */
void main_args(int argc, char* argv[], struct main_data* data) {
    //TODO: Check if file args are correct (contains all args)
    FILE* config = openFile(argv[1], "r");
    char buffer[CHAR_BUFFER_SIZE];

    readInt(config, &data->max_ops);
    readInt(config, &data->buffers_size);
    readInt(config, &data->n_restaurants);
    readInt(config, &data->n_drivers);
    readInt(config, &data->n_clients);

    readString(config, buffer);
    log_filename = create_dynamic_memory(strlen(buffer)+1);
    strcpy(log_filename, buffer);

    readString(config, buffer);
    statistics_filename = create_dynamic_memory(strlen(buffer)+1);
    strcpy(statistics_filename, buffer);

    alarm_time = create_dynamic_memory(sizeof(int));
    readInt(config, alarm_time);
    
    closeFile(config);
}

/* Função que reserva a memória dinâmica necessária para a execução
 * do MAGNAEATS, nomeadamente para os arrays *_pids e *_stats da estrutura
 * main_data. Para tal, pode ser usada a função create_dynamic_memory.
 */
void create_dynamic_memory_buffers(struct main_data* data) {
    data->restaurant_pids = create_dynamic_memory(data->n_restaurants * sizeof(int));
    data->driver_pids = create_dynamic_memory(data->n_drivers * sizeof(int));
    data->client_pids = create_dynamic_memory(data->n_clients * sizeof(int));
    data->restaurant_stats = create_dynamic_memory(data->max_ops * sizeof(int));
    data->driver_stats = create_dynamic_memory(data->max_ops * sizeof(int));
    data->client_stats = create_dynamic_memory(data->max_ops * sizeof(int));
}

/* Função que reserva a memória partilhada necessária para a execução do
 * MAGNAEATS. É necessário reservar memória partilhada para todos os buffers da
 * estrutura communication_buffers, incluindo os buffers em si e respetivos
 * pointers, assim como para o array data->results e variável data->terminate.
 * Para tal, pode ser usada a função create_shared_memory.
 */
void create_shared_memory_buffers(struct main_data* data, struct communication_buffers* buffers) {
    // main_rest buffer
    buffers->main_rest->ptrs = create_shared_memory(STR_SHM_MAIN_REST_PTR, sizeof(int) * data->buffers_size);
    buffers->main_rest->buffer = create_shared_memory(STR_SHM_MAIN_REST_BUFFER, sizeof(struct operation) * data->buffers_size);
    // rest_driv buffer
    buffers->rest_driv->ptrs = create_shared_memory(STR_SHM_REST_DRIVER_PTR, sizeof(struct pointers));
    buffers->rest_driv->buffer = create_shared_memory(STR_SHM_REST_DRIVER_BUFFER, sizeof(struct operation) * data->buffers_size);
    // driv_cli buffer
    buffers->driv_cli->ptrs = create_shared_memory(STR_SHM_DRIVER_CLIENT_PTR, sizeof(int) * data->buffers_size);
    buffers->driv_cli->buffer = create_shared_memory(STR_SHM_DRIVER_CLIENT_BUFFER, sizeof(struct operation) * data->buffers_size);
    // result and terminate
    data->results = create_shared_memory(STR_SHM_RESULTS, sizeof(struct operation) * data->max_ops);
    data->terminate = create_shared_memory(STR_SHM_TERMINATE, sizeof(int));
}

/* Função que inicia os processos dos restaurantes, motoristas e
 * clientes. Para tal, pode usar as funções launch_*,
 * guardando os pids resultantes nos arrays respetivos
 * da estrutura data.
 */
void launch_processes(struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    for (int i = 0; i < data->n_restaurants; i++) {
        *(data->restaurant_pids + i) = launch_restaurant(i, buffers, data, sems);
    }
    for (int j = 0; j < data->n_drivers; j++) {
        *(data->driver_pids + j) = launch_driver(j, buffers, data, sems);
    }
    for (int k = 0; k < data->n_clients; k++) {
        *(data->client_pids + k) = launch_client(k, buffers, data, sems);
    }
}

/* Função que faz interação do utilizador, podendo receber 4 comandos:
 * request - cria uma nova operação, através da função create_request
 * status - verifica o estado de uma operação através da função read_status
 * stop - termina o execução do MAGNAEATS através da função stop_execution
 * help - imprime informação sobre os comandos disponiveis
 */
void user_interaction(struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    char command[COMMAND_MAX_SIZE] = {0};
    int op_counter = 0;

    while (1) {
        scanf("%7s", command);

        if (strcmp(command, REQUEST) == 0) {
            create_request(&op_counter, buffers, data, sems);
        }
        if (strcmp(command, STATUS) == 0) {
            read_status(data, sems);
        }
        if (strcmp(command, STOP) == 0) {
            stop_execution(data, buffers, sems);
            return;
        }
        if (strcmp(command, HELP) == 0) {
            printf(COMMANDS, REQUEST, STATUS, STOP, HELP);
        }
    }
}

/* Se o limite de operações ainda não tiver sido atingido, cria uma nova
 * operação identificada pelo valor atual de op_counter e com os dados passados em
 * argumento, escrevendo a mesma no buffer de memória partilhada entre main e restaurantes.
 * Imprime o id da operação e incrementa o contador de operações op_counter.
 */
void create_request(int* op_counter, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    if (data->max_ops != *op_counter) {
        struct operation op;
        int cli;
        int rest;
        char dish[30] = {0};
        scanf("%d", &cli);
        scanf("%d", &rest);
        scanf("%29s", dish);
        op.id = *op_counter;
        op.status = 'I';
        op.requesting_client = cli;
        op.requested_rest = rest;
        op.requested_dish = dish;
        op.receiving_driver = -1;
        op.receiving_client = -1;
        semaphore_mutex_lock(sems->results_mutex);
        memcpy(data->results + op.id, &op, sizeof(struct operation));
        semaphore_mutex_unlock(sems->results_mutex);
        produce_begin(sems->main_rest);
        write_main_rest_buffer(buffers->main_rest, data->buffers_size, &op);
        produce_end(sems->main_rest);
        (*op_counter)++;
    } else {
        printf("Operation limit reached");
    }
}

/* Função que lê um id de operação do utilizador e verifica se a mesma
 * é valida. Em caso afirmativo,
 * imprime informação da mesma, nomeadamente o seu estado, o id do cliente
 * que fez o pedido, o id do restaurante requisitado, o nome do prato pedido
 * e os ids do restaurante, motorista, e cliente que a receberam e processaram.
 */
void read_status(struct main_data* data, struct semaphores* sems) {
    int id;
    scanf("%d", &id);
    semaphore_mutex_lock(sems->results_mutex);
    if ((data->results + id)->requested_dish != NULL) {
        printf("Status:%c\n", (data->results + id)->status);
        printf("Requested restaurant:%d\n", (data->results + id)->requested_rest);
        printf("Requesting client:%d\n", (data->results + id)->requesting_client);
        printf("Requested dish:%s\n", (data->results + id)->requested_dish);
        printf("Receiving restaurant:%d\n", (data->results + id)->receiving_rest);
        printf("Receiving driver:%d\n", (data->results + id)->receiving_driver);
        printf("Receiving client:%d\n", (data->results + id)->receiving_client);
    } else {
        printf("This operation does not exist");
    }
    semaphore_mutex_unlock(sems->results_mutex);
}

/* Função que termina a execução do programa MAGNAEATS. Deve começar por
 * afetar a flag data->terminate com o valor 1. De seguida, e por esta
 * ordem, deve esperar que os processos filho terminem, deve escrever as
 * estatisticas finais do programa, e por fim libertar
 * as zonas de memória partilhada e dinâmica previamente
 * reservadas. Para tal, pode usar as outras funções auxiliares do main.h.
 */
void stop_execution(struct main_data* data, struct communication_buffers* buffers, struct semaphores* sems) {
    *data->terminate = 1;
    wakeup_processes(data, sems);
    wait_processes(data);
    write_statistics(data);
    destroy_semaphores(sems);
    destroy_memory_buffers(data, buffers);
    // release memory before terminating
    destroy_dynamic_memory(data);
    destroy_dynamic_memory(buffers->main_rest);
    destroy_dynamic_memory(buffers->rest_driv);
    destroy_dynamic_memory(buffers->driv_cli);
    destroy_dynamic_memory(buffers);
}

/* Função que espera que todos os processos previamente iniciados terminem,
 * incluindo restaurantes, motoristas e clientes. Para tal, pode usar a função
 * wait_process do process.h.
 */
void wait_processes(struct main_data* data) {
    for (int i = 0; i < data->n_restaurants; i++) {
        *(data->restaurant_stats + i) = wait_process(*(data->restaurant_pids + i));
    }
    for (int j = 0; j < data->n_drivers; j++) {
        *(data->driver_stats + j) = wait_process(*(data->driver_pids + j));
    }
    for (int k = 0; k < data->n_clients; k++) {
        *(data->client_stats + k) = wait_process(*(data->client_pids + k));
    }
}

/* Função que imprime as estatisticas finais do MAGNAEATS, nomeadamente quantas
 * operações foram processadas por cada restaurante, motorista e cliente.
 */
void write_statistics(struct main_data* data) {
    printf("Restaurant stats:\n");
    for (int i = 0; i < data->n_restaurants; i++) {
        printf("%d : %d\n", i, *(data->restaurant_stats + i));
    }
    printf("Driver stats:\n");
    for (int j = 0; j < data->n_drivers; j++) {
        printf("%d : %d\n", j, *(data->driver_stats + j));
    }
    printf("Client stats:\n");
    for (int k = 0; k < data->n_clients; k++) {
        printf("%d : %d\n", k, *(data->client_stats + k));
    };
}

/* Função que liberta todos os buffers de memória dinâmica e partilhada previamente
 * reservados na estrutura data.
 */
void destroy_memory_buffers(struct main_data* data, struct communication_buffers* buffers) {
    //*_pids
    destroy_dynamic_memory(data->restaurant_pids);
    destroy_dynamic_memory(data->driver_pids);
    destroy_dynamic_memory(data->client_pids);
    //*_stats
    destroy_dynamic_memory(data->restaurant_stats);
    destroy_dynamic_memory(data->driver_stats);
    destroy_dynamic_memory(data->client_stats);
    // main_rest buffer
    destroy_shared_memory(STR_SHM_MAIN_REST_PTR, buffers->main_rest->ptrs, sizeof(int) * data->buffers_size);
    destroy_shared_memory(STR_SHM_MAIN_REST_BUFFER, buffers->main_rest->buffer, sizeof(struct operation) * data->buffers_size);
    // rest_driv buffer
    destroy_shared_memory(STR_SHM_REST_DRIVER_PTR, buffers->rest_driv->ptrs, sizeof(struct pointers));
    destroy_shared_memory(STR_SHM_REST_DRIVER_BUFFER, buffers->rest_driv->buffer, sizeof(struct operation) * data->buffers_size);
    // driv_cli buffer
    destroy_shared_memory(STR_SHM_DRIVER_CLIENT_PTR, buffers->driv_cli->ptrs, sizeof(int) * data->buffers_size);
    destroy_shared_memory(STR_SHM_DRIVER_CLIENT_BUFFER, buffers->driv_cli->buffer, sizeof(struct operation) * data->buffers_size);
    // result and terminate
    destroy_shared_memory(STR_SHM_RESULTS, data->results, sizeof(struct operation));
    destroy_shared_memory(STR_SHM_TERMINATE, data->terminate, sizeof(int));
}

void create_semaphores(struct main_data* data, struct semaphores* sems) {
    sems->main_rest->full = semaphore_create(STR_SEM_MAIN_REST_FULL, 0);
    sems->main_rest->empty = semaphore_create(STR_SEM_MAIN_REST_EMPTY, data->buffers_size);
    sems->main_rest->mutex = semaphore_create(STR_SEM_MAIN_REST_MUTEX, 1);

    sems->rest_driv->full = semaphore_create(STR_SEM_REST_DRIV_FULL, 0);
    sems->rest_driv->empty = semaphore_create(STR_SEM_REST_DRIV_EMPTY, data->buffers_size);
    sems->rest_driv->mutex = semaphore_create(STR_SEM_REST_DRIV_MUTEX, 1);

    sems->driv_cli->full = semaphore_create(STR_SEM_DRIV_CLI_FULL, 0);
    sems->driv_cli->empty = semaphore_create(STR_SEM_DRIV_CLI_EMPTY, data->buffers_size);
    sems->driv_cli->mutex = semaphore_create(STR_SEM_DRIV_CLI_MUTEX, 1);

    sems->results_mutex = semaphore_create(STR_SEM_RESULTS_MUTEX, 1);
}

void wakeup_processes(struct main_data* data, struct semaphores* sems) {
}

void destroy_semaphores(struct semaphores* sems) {
    semaphore_destroy(STR_SEM_MAIN_REST_FULL, sems->main_rest->full);
    semaphore_destroy(STR_SEM_MAIN_REST_EMPTY, sems->main_rest->empty);
    semaphore_destroy(STR_SEM_MAIN_REST_MUTEX, sems->main_rest->mutex);

    semaphore_destroy(STR_SEM_REST_DRIV_FULL, sems->rest_driv->full);
    semaphore_destroy(STR_SEM_REST_DRIV_EMPTY, sems->rest_driv->empty);
    semaphore_destroy(STR_SEM_REST_DRIV_MUTEX, sems->rest_driv->mutex);

    semaphore_destroy(STR_SEM_DRIV_CLI_FULL, sems->driv_cli->full);
    semaphore_destroy(STR_SEM_DRIV_CLI_EMPTY, sems->driv_cli->empty);
    semaphore_destroy(STR_SEM_DRIV_CLI_MUTEX, sems->driv_cli->mutex);

    semaphore_destroy(STR_SEM_RESULTS_MUTEX, sems->results_mutex);
}


int main(int argc, char* argv[]) {
    // init data structures
    struct main_data* data = create_dynamic_memory(sizeof(struct main_data));
    struct communication_buffers* buffers = create_dynamic_memory(sizeof(struct communication_buffers));
    buffers->main_rest = create_dynamic_memory(sizeof(struct rnd_access_buffer));
    buffers->rest_driv = create_dynamic_memory(sizeof(struct circular_buffer));
    buffers->driv_cli = create_dynamic_memory(sizeof(struct rnd_access_buffer));
    struct semaphores* sems = create_dynamic_memory(sizeof(struct semaphores));
    sems->driv_cli = create_dynamic_memory(sizeof(struct prodcons));
    sems->main_rest = create_dynamic_memory(sizeof(struct prodcons));
    sems->rest_driv = create_dynamic_memory(sizeof(struct prodcons));


    // execute main code
    if (argc == 2) {
        main_args(argc, argv, data);
        create_semaphores(data, sems);
        create_dynamic_memory_buffers(data);
        create_shared_memory_buffers(data, buffers);

        launch_processes(buffers, data, sems);
        printf(COMMANDS, REQUEST, STATUS, STOP, HELP);
        user_interaction(buffers, data, sems);
    } else {
        printf("Invalid starting arguments: not enough arguments");
    }
}