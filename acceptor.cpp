
#include "acceptor.h"
#include "endpoint.h"
#include "socket.h"
#include "poll.h"

int acceptor_t::open(){
    _svrfd = socket(AF_INET, SOCK_STREAM,0);
    assert(_svrfd>0);
    set_unblocking(_svrfd, 1);
    set_reuseaddr(_svrfd, 1);

    bind_address(_svrfd, _ip, _port);
    listen(_svrfd, BACK_LOG_SIZE);
    _ep->update(EPOLL_CTL_ADD, _svrfd, EPOLLIN | EPOLLET, this);
    return 0;
}

int acceptor_t::close() {
    exit(-1);
}

int acceptor_t::read() {
    int fd = accept(_svrfd);
    if(fd<0){
        return -1;
    }
    printf("%d accept client fd:%d\n", _svrfd, fd);
    endpoint_t *h = new endpoint_t(_ep, fd, _svr);
    h->open();
    return 0;
}

int acceptor_t::write() {
    return 0;
}
