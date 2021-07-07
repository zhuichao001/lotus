#include "util.h"

long long get_nanosec() {
    struct timespec tn;
    clock_gettime(CLOCK_REALTIME, &tn);
    return tn.tv_nsec;
}

long long get_millisec() {
    struct timespec tn;
    clock_gettime(CLOCK_REALTIME, &tn);
    return tn.tv_nsec / 1000000;
}

long long get_microsec() {
    struct timespec tn;
    clock_gettime(CLOCK_REALTIME, &tn);
    return tn.tv_nsec / 1000;
}
