#ifndef STATS_H_GUARD
#define STATS_H_GUARD

#include "file-private.h"
#include "main.h"
#include "metime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_BUFFER_SIZE 100

void writeStats(struct main_data* data, char* fileName, int op_counter);

#endif