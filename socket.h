#ifndef _NET_SOCKET_H_
#define _NET_SOCKET_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>
#include <sys/ioctl.h>

int bind_address(int fd, const char* ip, int port);

int set_reuseaddr(int fd, int reuse);

int set_unblocking(int fd, int on);

int accept(int svrfd);

#endif
