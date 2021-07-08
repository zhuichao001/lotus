
#include "acceptor.h"
#include "endpoint.h"
#include "socket.h"
#include "poll.h"

int acceptor_t::open(){
    _fd = socket(AF_INET, SOCK_STREAM,0);
    assert(_fd>0);
    set_unblocking(_fd, 1);
    set_reuseaddr(_fd, 1);

    bind_address(_fd, _addr->ip.c_str(), _addr->port);
    ::listen(_fd, BACK_LOG_SIZE);
    _ep->update(EPOLL_CTL_ADD, _fd, EPOLLIN | EPOLLET, this);
    return 0;
}

int acceptor_t::close() {
    exit(-1);
    return 0;
}

int acceptor_t::read() {
    int cfd = ::accept(_fd);
    if(cfd<0){
        return -1;
    }
    printf("%d accept client fd:%d.\n", _fd, cfd);
    endpoint_t *h = new endpoint_t(_ep, cfd, _svr);
    h->open();
    return 0;
}
