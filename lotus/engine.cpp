#include "engine.h"

engine_t::engine_t(): 
    _running(true){
    _ep = new evloop_t(MAX_CONN_NUMS);
    _tracker = new timetracker_t(_ep); 
}

engine_t::~engine_t(){
    for(auto it : _listeners){
        delete(it.second);
    }
    for(auto it : _clients){
        delete(it.second);
    }
    delete _tracker;
    delete _ep;
}

void engine_t::heartbeat(){
    fprintf(stderr, "[lotus] heartbeat clock at:%ld\n", microsec());
}

//boot new server
int engine_t::start(const address_t *addr, server_t* svr){
    acceptor_t *ac = new acceptor_t(_ep, addr, svr);
    ac->open();
    _listeners[ac->fd()] = ac;
    
    //TODO DELETE
    //run_every(std::bind(&engine_t::heartbeat, this), 1000*1000);
    return 0;
}

void engine_t::stop(){
    _running = false;
}

//boot new client
dialer_t * engine_t::open(const address_t *addr){
    dialer_t *cli = new dialer_t(_ep, addr);
    _clients[cli->fd()] = cli;
    return cli;
}

int engine_t::run(){
    while (_running) {
        _ep->loop();
    }
}

lotus::timer_t *engine_t::run_at(timer_callback_t cb, uint64_t when) {
    return _tracker->add(cb, when, 0);
}

lotus::timer_t *engine_t::run_after(timer_callback_t cb, uint64_t delay) {
    return _tracker->add(cb, microsec()+delay, 0);
}

lotus::timer_t *engine_t::run_every(timer_callback_t cb, uint64_t interval) {
    return _tracker->add(cb, microsec()+interval, interval);
}
