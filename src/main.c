#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "process.h"

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

/* Função que lê os argumentos da aplicação, nomeadamente o número
 * máximo de operações, o tamanho dos buffers de memória partilhada
 * usados para comunicação, e o número de clientes, de motoristas e de
 * restaurantes. Guarda esta informação nos campos apropriados da
 * estrutura main_data.
 */
void main_args(int argc, char* argv[], struct main_data* data) {
    // TODO: check if enough args
    data->max_ops = atoi(argv[1]);
    data->buffers_size = atoi(argv[2]);
    data->n_restaurants = atoi(argv[3]);
    data->n_drivers = atoi(argv[4]);
    data->n_clients = atoi(argv[5]);
    // hardcoded, argc doesnt make much sense ? check main
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
    buffers->main_rest->ptrs = create_shared_memory(STR_SHM_MAIN_REST_PTR, sizeof(int));
    buffers->main_rest->buffer = create_shared_memory(STR_SHM_MAIN_REST_BUFFER, data->buffers_size);
    // rest_driv buffer
    buffers->rest_driv->ptrs = create_shared_memory(STR_SHM_REST_DRIVER_PTR, sizeof(struct pointers));
    buffers->rest_driv->buffer = create_shared_memory(STR_SHM_REST_DRIVER_BUFFER, data->buffers_size);
    // driv_cli buffer
    buffers->driv_cli->ptrs = create_shared_memory(STR_SHM_DRIVER_CLIENT_PTR, sizeof(int));
    buffers->driv_cli->buffer = create_shared_memory(STR_SHM_DRIVER_CLIENT_BUFFER, data->buffers_size);
    // result and terminate
    data->results = create_shared_memory(STR_SHM_RESULTS, sizeof(struct operation) * data->max_ops);
    // TODO Char pointer
    data->terminate = create_shared_memory(STR_SHM_TERMINATE, sizeof(int));
}

/* Função que inicia os processos dos restaurantes, motoristas e
 * clientes. Para tal, pode usar as funções launch_*,
 * guardando os pids resultantes nos arrays respetivos
 * da estrutura data.
 */
void launch_processes(struct communication_buffers* buffers, struct main_data* data) {
    for (int i = 0; i < data->n_restaurants; i++) {
        *(data->restaurant_pids + i) = launch_restaurant(i, buffers, data);
    }
    for (int j = 0; j < data->n_drivers; j++) {
        *(data->driver_pids + j) = launch_driver(j, buffers, data);
    }
    for (int k = 0; k < data->n_clients; k++) {
        *(data->client_pids + k) = launch_client(k, buffers, data);
    }
}

/* Função que faz interação do utilizador, podendo receber 4 comandos:
 * request - cria uma nova operação, através da função create_request
 * status - verifica o estado de uma operação através da função read_status
 * stop - termina o execução do MAGNAEATS através da função stop_execution
 * help - imprime informação sobre os comandos disponiveis
 */
void user_interaction(struct communication_buffers* buffers, struct main_data* data) {
    char command[COMMAND_MAX_SIZE] = {0};
    int op_counter = 0;

    while (1) {
        scanf("%7s", command);

        if (strcmp(command, REQUEST) == 0) {
            create_request(&op_counter, buffers, data);
        }
        if (strcmp(command, STATUS) == 0) {
            read_status(data);
        }
        if (strcmp(command, STOP) == 0) {
            write_statistics(data);
            stop_execution(data, buffers);
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
void create_request(int* op_counter, struct communication_buffers* buffers, struct main_data* data) {
    if (data->max_ops != *op_counter) {
        struct operation op;
        int cli;
        int rest;
        char dish[30] = {0};
        scanf("%[^\n]d", cli);
        scanf("%[^\n]d", rest);
        scanf("%[^\n]s", dish);
        op.id = *op_counter;
        op.status = 'I';
        op.requesting_client = cli;
        op.requested_rest = rest;
        op.requested_dish = dish;
        memcpy(data->results + op.id, &op, sizeof(struct operation));
        write_main_rest_buffer(buffers->main_rest, data->buffers_size, &op);
        printf("%d", op.id);
        *op_counter++;
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
void read_status(struct main_data* data) {
    int id;
    scanf("%[^\n]d", id);
    if ((data->results + id) != NULL) {
        printf("Status:%c\n", (data->results + id)->status);
    } else {
        printf("This operation does not exist");
    }
}

/* Função que termina a execução do programa MAGNAEATS. Deve começar por
 * afetar a flag data->terminate com o valor 1. De seguida, e por esta
 * ordem, deve esperar que os processos filho terminem, deve escrever as
 * estatisticas finais do programa, e por fim libertar
 * as zonas de memória partilhada e dinâmica previamente
 * reservadas. Para tal, pode usar as outras funções auxiliares do main.h.
 */
void stop_execution(struct main_data* data, struct communication_buffers* buffers) {
    *data->terminate = 1;
    wait_processes(data);
    write_statistics(data);
    destroy_memory_buffers(data, buffers);
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
        printf("%d : %d\n", *(data->restaurant_pids + i), *(data->restaurant_stats + i));
    }
    printf("Driver stats:\n");
    for (int j = 0; j < data->n_drivers; j++) {
        printf("%d : %d\n", *(data->driver_pids + j), *(data->driver_stats + j));
    }
    printf("Client stats:\n");
    for (int k = 0; k < data->n_clients; k++) {
        printf("%d : %d\n", *(data->client_pids + k), *(data->client_stats + k));
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
    destroy_shared_memory(STR_SHM_MAIN_REST_PTR, buffers->main_rest->ptrs, sizeof(int));
    destroy_shared_memory(STR_SHM_MAIN_REST_BUFFER, buffers->main_rest->buffer, data->buffers_size);
    // rest_driv buffer
    destroy_shared_memory(STR_SHM_REST_DRIVER_PTR, buffers->rest_driv->ptrs, sizeof(struct pointers));
    destroy_shared_memory(STR_SHM_REST_DRIVER_BUFFER, buffers->rest_driv->buffer, data->buffers_size);
    // driv_cli buffer
    destroy_shared_memory(STR_SHM_DRIVER_CLIENT_PTR, buffers->driv_cli->ptrs, sizeof(int));
    destroy_shared_memory(STR_SHM_DRIVER_CLIENT_BUFFER, buffers->driv_cli->buffer, data->buffers_size);
    // result and terminate
    destroy_shared_memory(STR_SHM_RESULTS, data->results, sizeof(struct operation));
    destroy_shared_memory(STR_SHM_TERMINATE, data->terminate, sizeof(int));
}

int main(int argc, char* argv[]) {
    // init data structures
    struct main_data* data = create_dynamic_memory(sizeof(struct main_data));
    struct communication_buffers* buffers = create_dynamic_memory(sizeof(struct communication_buffers));
    buffers->main_rest = create_dynamic_memory(sizeof(struct rnd_access_buffer));
    buffers->rest_driv = create_dynamic_memory(sizeof(struct circular_buffer));
    buffers->driv_cli = create_dynamic_memory(sizeof(struct rnd_access_buffer));

    // execute main code
    main_args(argc, argv, data);
    create_dynamic_memory_buffers(data);
    create_shared_memory_buffers(data, buffers);
    launch_processes(buffers, data);
    printf(COMMANDS, REQUEST, STATUS, STOP, HELP);
    user_interaction(buffers, data);

    // release memory before terminating
    destroy_dynamic_memory(data);
    destroy_dynamic_memory(buffers->main_rest);
    destroy_dynamic_memory(buffers->rest_driv);
    destroy_dynamic_memory(buffers->driv_cli);
    destroy_dynamic_memory(buffers);
}