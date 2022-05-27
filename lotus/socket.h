#pragma once

#include <stdio.h>
#include <errno.h>
#include <string>
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
#include "buff.h"

int bind_address(int fd, const char* ip, int port);

int set_reuseaddr(int fd, int reuse);

int set_reuseport(int fd, bool on);

int set_unblocking(int fd, int on);

int set_keepalive(int fd, bool on);

int set_nodelay(int fd, bool on);

int accept(int svrfd);

int connect(const char *ip, int port);

int bread(int fd, buff_t *rb);

int bwrite(int fd, buff_t *wb);

void get_peer_ip_port(int fd, std::string *ip, int *port);

int get_tcpinfo(struct tcp_info *info);

bool get_tcpinfo_string(int fd, char* buf, int len);
