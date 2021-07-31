#ifndef _NET_TIMER_H_
#define _NET_TIMER_H_

#include "types.h"

namespace lotus {

class timer_t {
public:
    timer_t(timer_callback_t cb, uint64_t when, uint64_t interval):
        _callback(cb),
        _expire(when),
        _interval(interval),
        _canceled(false) {
        _repeatable = _interval>0 ? true:false;
    }

    void run(){
        if(!_canceled){
            return;
        }
        _callback();
    }

    void cancel(){
        _canceled = true;
    }

    bool repeatable()const{
        return _repeatable;
    }

    timer_t *next(){
        _expire += _interval;
        return this;
    }

    uint64_t expireat()const{
        return _expire; 
    }

    bool operator<(const timer_t& t)const{ return _expire < t._expire; }
    bool operator==(const timer_t& t)const{ return _expire == t._expire; }
private:
    const timer_callback_t _callback;
    uint64_t _expire;
    uint64_t _interval;
    bool _canceled;
    bool _repeatable;
};

}; //end of namespace lotus

#endif
