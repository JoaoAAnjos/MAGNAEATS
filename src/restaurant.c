#include "restaurant.h"

#include <stdio.h>
#include <string.h>
#include "metime.h"

/* Função principal de um Restaurante. Deve executar um ciclo infinito onde em
 * cada iteração lê uma operação da main e se e data->terminate ainda for igual a 0, processa-a e
 * escreve-a para os motoristas. Operações com id igual a -1 são ignoradas
 * (op inválida) e se data->terminate for igual a 1 é porque foi dada ordem
 * de terminação do programa, portanto deve-se fazer return do número de
 * operações processadas. Para efetuar estes passos, pode usar os outros
 * métodos auxiliares definidos em restaurant.h.
 */
int execute_restaurant(int rest_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    int counter = 0;
    while (1) {
        struct operation op;
        restaurant_receive_operation(&op, rest_id, buffers, data, sems);
        if (*data->terminate) {
            return counter;
        } else if (op.id != -1) {
            printf("Restaurante recebeu pedido!\n");
            restaurant_process_operation(&op, rest_id, data, &counter, sems);
            restaurant_forward_operation(&op, buffers, data, sems);
        }
    }
}

/* Função que lê uma operação do buffer de memória partilhada entre
 * a Main e os restaurantes que seja direcionada ao restaurante rest_id.
 * Antes de tentar ler a operação, o processo deve
 * verificar se data->terminate tem valor 1.
 * Em caso afirmativo, retorna imediatamente da função.
 */
void restaurant_receive_operation(struct operation* op, int rest_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    consume_begin(sems->main_rest);
    if (!*data->terminate) {
        read_main_rest_buffer(buffers->main_rest, rest_id, data->buffers_size, op);
    } else {
        return;
    }
    
    consume_end(sems->main_rest);
    getTime(&op->rest_time);
}

/* Função que processa uma operação, alterando o seu campo receiving_rest para o id
 * passado como argumento, alterando o estado da mesma para 'R', e
 * incrementando o contador de operações. Atualiza também a operação na estrutura data.
 */
void restaurant_process_operation(struct operation* op, int rest_id, struct main_data* data, int* counter, struct semaphores* sems) {
    op->receiving_rest = rest_id;
    op->status = 'R';
    (*counter)++;
    semaphore_mutex_lock(sems->results_mutex);
    memcpy(data->results + op->id, op, sizeof(struct operation));
    semaphore_mutex_unlock(sems->results_mutex);
}

/* Função que escreve uma operação no buffer de memória partilhada entre
 * restaurantes e motoristas, efetuando a necessária sincronização antes e
 * depois de escrever.
 */
void restaurant_forward_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    produce_begin(sems->rest_driv);
    write_rest_driver_buffer(buffers->rest_driv, data->buffers_size, op);
    produce_end(sems->rest_driv);
}