#include "util.h"

long long nanosec() {
    struct timespec tn;
    clock_gettime(CLOCK_REALTIME, &tn);
    return tn.tv_nsec;
}

long long millisec() {
    struct timespec tn;
    clock_gettime(CLOCK_REALTIME, &tn);
    return tn.tv_nsec / 1000000;
}

long long microsec() {
    struct timespec tn;
    clock_gettime(CLOCK_REALTIME, &tn);
    return tn.tv_nsec / 1000;
}
