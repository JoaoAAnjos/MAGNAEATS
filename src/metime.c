#include <metime.h>

void getTime(struct timespec* time) {
    if (clock_gettime(CLOCK_REALTIME, time) == -1) {
        printf("error in clock_gettime");
        exit(-1);
    }
}