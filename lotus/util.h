#pragma once

#include <string>
#include <time.h>

long long nanosec() ;
long long millisec() ;
long long microsec() ;

int localip(const std::string &name, std::string &ip);

void str2hex(const char *data, int len, char *dst);

char *str2upper(char *data);

//the length of `to` must be no less than `raw`
int copy_until(const char* &raw, const char *end, char *to, int tolen);
