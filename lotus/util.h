#ifndef _LOTUS_NET_UTIL_H_
#define _LOTUS_NET_UTIL_H_

#include <string>
#include <time.h>

long long nanosec() ;
long long millisec() ;
long long microsec() ;

int localip(const std::string &name, std::string &ip);

#endif
