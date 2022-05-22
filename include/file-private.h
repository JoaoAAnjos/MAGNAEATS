#ifndef FILE_H_GUARD_PRIVATE
#define FILE_H_GUARD_PRIVATE

#include <stdio.h>
#include <stdlib.h>

/* Função que abre ficheiro passado em argumento
 */
FILE* openFile(char* name, char* mode);

/* Função que fecha ficheiro passado em argumento
 */
void closeFile(FILE* file);

#endif