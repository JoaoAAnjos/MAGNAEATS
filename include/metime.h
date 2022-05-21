#ifndef METIME_H_GUARD
#define METIME_H_GUARD

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

/* Função que obtém o tempo atual do sistema 
*/
void getTime(struct timespec* time);

#endif