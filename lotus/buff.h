#ifndef _NET_BUFFER_H_
#define _NET_BUFFER_H_

#include <algorithm>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>


class buff_t {
public:
    buff_t(int cap):
        _capacity(cap), 
        _start(0), 
        _end(0){
        _buff = (char*)malloc(_capacity);
    }

    ~buff_t(){
        if(_buff!=NULL){
            free(_buff);
        }
    }

    void reset(){
        _start = 0;
        _end = 0;
    }

    bool empty(){
        return _start == _end;
    }

    int avaliable(char **data, int *len){
        if(*data==nullptr){
            *data =  _buff + _end;
        }
        *len = _capacity - _end;
        return 0;
    }

    int load(char **data, int *len){
        assert(data!=nullptr);
        *data =  _buff + _start;
        *len = _end - _start;
        return 0;
    }

    char *data(){
        return _buff + _start;
    }

    int len(){
        return _end - _start;
    }

    int rest(){
        return _capacity-len();
    }

    int release(int n){
        _start += n;
    }

    void expend(int right){
        _end += right;
    }

    void expand(int size=1024){
        _capacity += size;
        _buff = (char*)realloc(_buff, _capacity);
    }

    int append(const char *slice, int size){
        if(slice==nullptr || size<=0){
            return -1;
        }
        int length = this->len();
        if(size > _capacity-_end){
            if(rest()>=size){
                memcpy(_buff, _buff+_start, length);
                _start = 0;
                _end = length;
            }else{
                expand(std::max(size*2, _capacity/4));
            }
        }
        memcpy(_buff+_end, slice, size);
        _end += size;
        return size;
    }

    int append(buff_t *b){
        append(b->data(), b->len());
    }

private:
    char *_buff;
    int _capacity;
    int _start;
    int _end;
};

#endif
