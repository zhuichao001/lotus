#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "iohandler.h"

int iohandler_t::read(){
    char *data = nullptr;
    int len = 0;
    while(true){
        _rb.avaliable(&data, &len);
        int n = ::read(_fd, data, len);
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

        _rb.expend(n);
        if(n<len){  //normal
            fprintf(stdout,"%d read done.\n", _fd);
            break;
        }else if(len-n==0){
            _rb.expand();
        }
    }
    _rb.used(&data, &len);
    fprintf(stdout, "%d totally read:%s\n", _fd, data);
    return 0;
}

int iohandler_t::send(buff_t *out){
    _wb.append(out);
    return write();
}

int iohandler_t::write(){
    while(!_wb.empty()){
        char *data = nullptr;
        int len = 0;
        _wb.used(&data, &len); 

        int n = ::write(_fd, (void *)data, (size_t)len);
        if (n<0 && errno == EAGAIN) { //tcp buffer is full
            fprintf(stderr,"fd:%d write EAGAIN.\n", _fd);
            break;
        } else if (n<=0) { //error
            fprintf(stderr,"client: write errno:%d.\n", errno);
            return -1;
        } else {
            fprintf(stderr, "ok write: %s\n", data);
            this->_wb.repay(n); //return space
        }
    }
    return 0;
}
