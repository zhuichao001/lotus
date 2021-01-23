#ifndef _NET_BUFFER_H_
#define _NET_BUFFER_H_

#include <stdlib.h>
#include <memory.h>


class buff_t {
public:
    buff_t(int cap):_capacity(cap), _start(0), _end(0){
        _buff = (char*)malloc(_capacity);
    }

    ~buff_t(){
        if(_buff!=NULL){
            free(_buff);
        }
    }

    void move(int left, int right){
        _start += left;
        _end += right;
    }

    void reset(){
        _start = 0;
        _end = 0;
    }

    void finish(int n){
        _start += n;
    }

    bool empty(){
        return _start == _end;
    }

    char *data(){
        return _buff + _start;
    }

    int len(){
        return _end - _start;
    }

    int rest(char **data, int *len){
        *data =  _buff + _end;
        *len = _capacity - _end;
        return 0;
    }

    int used(char **data, int *len){
        *data =  _buff + _start;
        *len = _end - _start;
        return 0;
    }

    int release(int n){
        _start += n;
    }

    void expand(){
        _capacity <<= 1;
        _buff = (char*)realloc(_buff, _capacity);
    }


    int append(char *data, int len){
        if(data==nullptr || len<=0){
            return -1;
        }
        if(len>_capacity-_end){
            return -1;
        }
        memcpy(_buff+_end, data, len);
        _end += len;
        return len;
    }

private:
    char *_buff;
    int _capacity;
    int _start;
    int _end;
};

#endif
