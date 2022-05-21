#ifndef LOG_H_GUARD
#define LOG_H_GUARD

#include "file-private.h"
#include "metime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_BUFFER_SIZE 100

void logInstruction(char* fileName, char* command);

void logArguments(char* fileName, char* arguments);

#endif