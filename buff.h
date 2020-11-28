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
        _capacity = 0;
        _start = 0;
        _end = 0;
    }

    bool empty(){
        return _start == _end;
    }

    int writable(char **data, int *len){
        *data =  _buff + _end;
        *len = _capacity - _end;
        return 0;
    }

    int readable(char **data, int *len){
        *data =  _buff + _start;
        *len = _end - _start;
        return 0;
    }

    void expand(){
        _capacity <<= 1;
        _buff = (char*)realloc(_buff, _capacity);
    }

private:
    char *_buff;
    int _capacity;
    int _start;
    int _end;
};

#endif
