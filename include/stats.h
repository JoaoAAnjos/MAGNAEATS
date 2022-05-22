#ifndef STATS_H_GUARD
#define STATS_H_GUARD

#include "file-private.h"
#include "main.h"
#include "metime.h"
#include <string.h>

void writeStats(struct main_data* data, char* fileName, int op_counter);

#endif