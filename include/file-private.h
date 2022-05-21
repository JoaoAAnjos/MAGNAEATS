#ifndef FILE_H_GUARD_PRIVATE
#define FILE_H_GUARD_PRIVATE

#include <stdio.h>
#include <stdlib.h>

/* 
*/
FILE* openFile(char* name, char* mode);

/*
*/
void closeFile(FILE* file);

#endif