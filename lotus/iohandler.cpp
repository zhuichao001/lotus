#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "iohandler.h"

int iohandler_t::read(){
    int total = 0;
    while(true){
        char *data = nullptr;
        int len = 0;
        _rb.avaliable(&data, &len);
        if(len==0){
            _rb.expand();
            continue;
        }

        int n = ::read(_fd, data, len);
        fprintf(stderr, "------- fd:%d read in %d byptes\n", _fd, n);
        if(n<0 && errno==EAGAIN){// read done
            fprintf(stderr, "%d read again.\n", _fd);
            return 0;
        }else if(n==0){   //conn closed
            fprintf(stderr, "%d read closed.\n", _fd);
            return -1;
        }else if(n<0){    //read error
            fprintf(stderr, "%d read errno:%d.\n", _fd, errno);
            return -1;
        }

        total += n;
        _rb.expend(n);
        if(n<len){  //normal
            fprintf(stdout,"%d read done.\n", _fd);
            break;
        }
    }
    fprintf(stdout, "%d totally read:%d bytes\n", _fd, total);
    return 0;
}

int iohandler_t::send(buff_t *buff){
    _wb.append(buff);
    this->write();
    if(!_wb.empty()){
        _ep->update(EPOLL_CTL_ADD, _fd, EPOLLOUT, (void*)this);
    }
    return 0;
}

int iohandler_t::write(){
    while(!_wb.empty()){
        char *data = nullptr;
        int len = 0;
        _wb.load(&data, &len); 

        int n = ::write(_fd, (void *)data, (size_t)len);
        fprintf(stderr, "+++++++ fd:%d write out %d byptes\n", _fd, n);
        if (n<0 && errno == EAGAIN) { //tcp buffer is full
            fprintf(stderr,"fd:%d write EAGAIN.\n", _fd);
            return 0;
        } else if (n<=0) { //error
            fprintf(stderr,"client: write errno:%d.\n", errno);
            return -1;
        } 
        fprintf(stderr, "ok write: %d bytes\n", len);
        this->_wb.release(n); //return space
        if(n<len){
            break;
        }
    }
    printf("return write\n");
    return 0;
}
