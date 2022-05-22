#include "client.h"

#include <stdio.h>
#include <string.h>
#include "metime.h"

/* Função principal de um Cliente. Deve executar um ciclo infinito onde em
 * cada iteração lê uma operação dos motoristas e se e data->terminate ainda
 * for igual a 0, processa-a. Operações com id igual a -1 são ignoradas
 * (op inválida) e se data->terminate for igual a 1 é porque foi dada ordem
 * de terminação do programa, portanto deve-se fazer return do número de
 * operações processadas. Para efetuar estes passos, pode usar os outros
 * métodos auxiliares definidos em client.h.
 */
int execute_client(int client_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    int op_proc = 0;
    while (1) {
        struct operation op;
        client_get_operation(&op, client_id, buffers, data, sems);
        if (*data->terminate) {
            return op_proc;
        } else if (op.id != -1) {
            printf("Cliente recebeu pedido!\n");
            client_process_operation(&op, client_id, data, &op_proc, sems);
        }
    }
}

/* Função que lê uma operação do buffer de memória partilhada entre os motoristas
 * e clientes que seja direcionada a client_id. Antes de tentar ler a operação, deve
 * verificar se data->terminate tem valor 1. Em caso afirmativo, retorna imediatamente da função.
 */
void client_get_operation(struct operation* op, int client_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    if (!*data->terminate) {
        consume_begin(sems->driv_cli);
        read_driver_client_buffer(buffers->driv_cli, client_id, data->buffers_size, op);
        consume_end(sems->driv_cli);
        getTime(&op->client_end_time);
        if(op->id == (-1)){
            semaphore_mutex_unlock(sems->driv_cli->full);
            semaphore_mutex_lock(sems->driv_cli->empty);
        }
    } else {
        return;
    }
}

/* Função que processa uma operação, alterando o seu campo receiving_client para o id
 * passado como argumento, alterando o estado da mesma para 'C' (client), e
 * incrementando o contador de operações. Atualiza também a operação na estrutura data.
 */
void client_process_operation(struct operation* op, int client_id, struct main_data* data, int* counter, struct semaphores* sems) {
    op->receiving_client = client_id;
    op->status = 'C';
    (*counter)++;
    semaphore_mutex_lock(sems->results_mutex);
    memcpy(data->results + op->id, op, sizeof(struct operation));
    semaphore_mutex_unlock(sems->results_mutex);
}
