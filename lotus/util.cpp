#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <linux/if_link.h>
#include <memory.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"

long long nanosec() {
    struct timespec tn;
    clock_gettime(CLOCK_REALTIME, &tn);
    return tn.tv_nsec;
}

long long millisec() {
    struct timespec tn;
    clock_gettime(CLOCK_REALTIME, &tn);
    return tn.tv_sec*1000 + tn.tv_nsec / 1000000;
}

long long microsec() {
    struct timespec tn;
    clock_gettime(CLOCK_REALTIME, &tn);
    return tn.tv_sec*1000000 +tn.tv_nsec / 1000;
}

int localip(const std::string &name, std::string &ip){
    int sockfd;
    struct ifconf ifconf;
    struct ifreq *ifreq;
    char buf[512];

    ifconf.ifc_len = 512;
    ifconf.ifc_buf = buf;
    if ((sockfd =socket(AF_INET,SOCK_DGRAM,0))<0) {
        return -1;
    }

    ioctl(sockfd, SIOCGIFCONF, &ifconf);
    ifreq = (struct ifreq*)ifconf.ifc_buf;

    const int cnt = (ifconf.ifc_len/sizeof (struct ifreq));
    for (int i=cnt; i>0; i--) {
        if(ifreq[i].ifr_flags != AF_INET){ //for ipv4
            continue;
        }
        if(strcmp(name.c_str(), ifreq[i].ifr_name)==0){
            ip = inet_ntoa(((struct sockaddr_in*)&(ifreq[i].ifr_addr))->sin_addr);
            return 0;
        }
    }

    ip = inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr);
    return -1;
}

void str2hex(const char *data, int len, char *dst){
    //printf("%s, len=%d\n", data, len);
    for(int i=0; i<len; ++i){
        dst[i*2] = (data[i]&0X0F) + 'a';
        dst[i*2+1] = ((data[i]>>4)&0X0F) + 'a';
        //printf("%c %c \n", dst[i*2], dst[i*2+1]);
    }
    dst[len*2] = 0;
}
