#ifndef _NET_SOCKET_H_
#define _NET_SOCKET_H_

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>
#include <sys/ioctl.h>


int bind_address(int fd, const char* ip, int port);

int set_reuseaddr(int fd, int reuse);

int set_reuseport(int fd, bool on);

int set_unblocking(int fd, int on);

int set_keepalive(int fd, bool on);

int set_nodelay(int fd, bool on);

int accept(int svrfd);

int connect(const char *ip, int port);

int get_tcpinfo(struct tcp_info *info);

bool get_tcpinfo_string(int fd, char* buf, int len);

#endif
