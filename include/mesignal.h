#ifndef MESIGNAL_H_GUARD
#define MESIGNAL_H_GUARD

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "main.h"
#include "synchronization.h"

/*Função que inicializa o signal e signal_handler e flags
 * returna erro na eventualidade de uma inicialização incorreta
 */
void setup_stop(struct main_data* data, struct communication_buffers* buffers, struct semaphores* sems);

/*Função que é executada após receber o sinal de INT,
 * inicia o stop_execution com os argumentos passados na função setup_stop
 */
void handle_sigint(int sig);

/*Função que inicializa o alarme de tempo, correndo
 * a função handle_alarm com intervalo de tempo passado pelos argumentos
 */
void set_alarm(int alarm_time, int* op_counter);

/*Função executada após receber o sinal SIGALARM,
 * da print do status atual dos pedidos
 */
void handle_alarm();

#endif