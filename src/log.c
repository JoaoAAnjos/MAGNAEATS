#include "log.h"

void logInstruction(char* fileName, char* command) {
    FILE* file = openFile(fileName, "a");
    struct timespec time;
    char buffer[CHAR_BUFFER_SIZE];

    getTime(&time);
    strftime(buffer, CHAR_BUFFER_SIZE, "%Y-%m-%d %H:%M:%S", localtime(&time.tv_sec));

    fprintf(file, "%s %s", buffer, command);
    if (strcmp(command, "stop") == 0 || strcmp(command, "help") == 0) {
        fprintf(file, "\n");
    }
    closeFile(file);
}

void logArguments(char* fileName, char* arguments) {
    FILE* file = openFile(fileName, "a");
    fprintf(file, " %s\n", arguments);
    closeFile(file);
}