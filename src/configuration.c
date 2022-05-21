#include "configuration.h"

void readInt(FILE* file, int* dest) {
    fscanf(file, "%d", dest);
}

void readString(FILE* file, char* dest) {
    fscanf(file, "%s", dest);
}