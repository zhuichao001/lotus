#pragma once

#include <algorithm>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>

class buff_t {
public:
    buff_t(int cap=2048):
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
        memset(_buff, 0, _capacity);
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
        if(_start==_end){
            reset();
        }
        return 0;
    }

    void expend(int right){
        _end += right;
    }

    void expand(int size=1024){
        _capacity += size;
        _buff = (char*)realloc(_buff, _capacity);
    }

    int append(const void *slice, int size){
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
        return 0;
    }

    //litte endian
    int read_le(void *val, int bytes){
        if(bytes>8 || len()<bytes){
            return -1;
        }
        memcpy(val, data(), bytes);
        release(bytes);
        return 0;
    }

    //big endian
    int read_be(void *val, int bytes){
        if(bytes>8 || len()<bytes){
            return -1;
        }
        uint8_t *pval = (uint8_t*)val;
        uint8_t *p = (unsigned char *)data()+bytes-1;
        for(int i=0; i<bytes; ++i){
            *(uint8_t*)val = *pval;
            --p;
            ++pval;
        }
        release(bytes);
        return 0;
    }

    //litte endian
    int write_le(const void *val, int bytes){
        if(bytes>8){
            return -1;
        }
        append((const char*)val, bytes);
        return 0;
    }

    //big endian
    int write_be(const void *val, int bytes){
        if(bytes>8){
            return -1;
        }
        for(int i=0; i<bytes; ++i){
            append((const char*)val+(bytes-1-i), 1);
        }
        return 0;
    }

private:
    char *_buff;
    int _capacity;

    int _start;
    int _end;
};
