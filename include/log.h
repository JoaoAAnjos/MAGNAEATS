#ifndef LOG_H_GUARD
#define LOG_H_GUARD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file-private.h"
#include "metime.h"

#define CHAR_BUFFER_SIZE 100

/**Função escreve a instrução para o ficheiro, passados nos argumentos,
 * juntamente com a data adequada em formato %Y-%m-%d %H:%M:%S
 */
void logInstruction(char* fileName, char* command);

/* Função escreve para o ficheiro os argumentos do comando,
 * caso seja um comando com argumentos tipo o request e o status
 */
void logArguments(char* fileName, char* arguments);

#endif