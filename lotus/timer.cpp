#include "timer.h"
#include "timetracker.h"

namespace lotus {

timer_t::timer_t(timetracker_t* trac, timer_callback_t cb, uint64_t when, uint64_t interval):
    _tracker(trac),
    _callback(cb),
    _expire(when),
    _interval(interval),
    _canceled(false) {
    _repeatable = _interval>0 ? true:false;
}

void timer_t::fired(){
    if(_canceled){
        return;
    }
    _callback();
}

int timer_t::cancel(){
    return _tracker->del(this);
}

int timer_t::update(uint64_t expireat){
    if(_tracker->del(this)<0){
        return -1;
    }
    _expire = expireat;
    _tracker->add(this);
    return 0;
}

bool timer_t::repeatable()const{
    return _repeatable;
}

timer_t *timer_t::next(){
    _expire += _interval;
    return this;
}

uint64_t timer_t::expireat()const{
    return _expire; 
}

}
