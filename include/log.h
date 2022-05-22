#ifndef LOG_H_GUARD
#define LOG_H_GUARD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file-private.h"
#include "metime.h"

#define CHAR_BUFFER_SIZE 100

/**Função da print da instrução para o ficheiro passados nos argumentos
 * juntamente com a data adequada em formato %Y-%m-%d %H:%M:%S
 */
void logInstruction(char* fileName, char* command);

void logArguments(char* fileName, char* arguments);

#endif