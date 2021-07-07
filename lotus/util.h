#ifndef _NET_UTIL_H_
#define _NET_UTIL_H_

#include <time.h>
using namespace std;

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

#endif
