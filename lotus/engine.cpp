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
int engine_t::start(const address_t *addr, service_t* svr){
    acceptor_t *ac = new acceptor_t(_ep, addr, svr);
    ac->open();
    _listeners[ac->fd()] = ac;
    return 0;
}

void engine_t::stop(){
    _running = false;
}

//boot new client
dialer_t * engine_t::open(const address_t *addr){
    dialer_t *cli = new dialer_t(_ep, addr, this);
    if(cli->usable()){
        _clients[cli->fd()] = cli;
        return cli;
    } else {
        delete cli;
        return nullptr;
    }
}

int engine_t::run(){
    while (_running) {
        _ep->loop();
    }
    return 0;
}

lotus::timer_t *engine_t::run_at(timer_callback_t cb, uint64_t when) {
    lotus::timer_t* t = new lotus::timer_t(_tracker, cb, when, 0);
    _ep->post(std::bind(&timetracker_t::add, _tracker, t));
    return t;
}

lotus::timer_t *engine_t::run_after(timer_callback_t cb, uint64_t delay) {
    uint64_t when = microsec()+delay;
    lotus::timer_t* t = new lotus::timer_t(_tracker, cb, when, 0);
    _ep->post(std::bind(&timetracker_t::add, _tracker, t));
    return t;
}

lotus::timer_t *engine_t::run_every(timer_callback_t cb, uint64_t interval) {
    uint64_t when = microsec()+interval;
    lotus::timer_t* t = new lotus::timer_t(_tracker, cb, when, interval);
    _ep->post(std::bind(&timetracker_t::add, _tracker, t));
    return t;
}
