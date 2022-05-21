#include "file-private.h"

FILE* openFile(char* name, char* mode) {
    FILE* file = fopen(name, mode);
    if (file == NULL || ferror(file) != 0) {
        perror("error in fopen");
        exit(-1);
    }
    return file;
}

void closeFile(FILE* file) {
    fclose(file);
}