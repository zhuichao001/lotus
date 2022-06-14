#pragma once

#include "endpoint.h"
#include "protocol/mysql.h"

template<>
template<sql_request_t, sql_response_t>
class endpoint_t: 
    public iohandler_t {
public:
    enum side_type_t{
        CLIENT_SIDE = 1,
        SERVER_SIDE = 2,
    };

    endpoint_t(side_type_t side, evloop_t *ep, int fd, comhandler_t *ch):
        _side(side),
        _ep(ep), 
        _fd(fd),
        _com(ch),
        _stage(STAGE::HANDSHAKE),
        _rb(2048), 
        _wb(4096){
    }

    int open(){
        assert(_fd>0);
        set_reuseaddr(_fd, 1);
        set_unblocking(_fd, 1);
        _ep->update(EPOLL_CTL_ADD, _fd, EPOLLIN | EPOLLET, (void*)this);
        return 0;
    }

    int close(){
        _wb.reset();
        _rb.reset();

        fprintf(stderr, "endpoint closed, fd:%d\n", _fd);

        if(_fd>0){
            _ep->update(EPOLL_CTL_DEL, _fd, 0, nullptr);
            ::close(_fd);
        }
        _fd = -1;

        _com->onclose();
        return 0;
    }

    int read() override {
        bread(_fd, &_rb);
        _ep->post(std::bind(&endpoint_t::receive, this));
        return 0;
    }

    int write() override{
        if(_wb.empty()){
            fprintf(stderr, "Warning: write empty\n");
            return -1;
        }

        if(!_wb.empty()){
            bwrite(_fd, &_wb);
        }
        if(!_wb.empty()){
            _ep->update(EPOLL_CTL_ADD, _fd, EPOLLOUT, (void*)this);
        }
        return 0;
    }

    int send(buff_t *buf){
        assert(buf!=nullptr);
        _wb.append(buf);
        write();
        return 0;
    }

    int fd(){
        return _fd;
    }

    bool established(){
        return _fd>0;
    }

    STAGE stage(){
        return _stage;
    }

private:
    int receive(){
        if(_side==side_type_t::SERVER_SIDE){
            if(_stage==STAGE::HANDSHAKE){
                auth_packet_t pkt;
                int n = pkt.decode(&_rb);
                if(n<0){ //failed
                    fprintf(stderr, "Error: request decode failed\n");
                    return -1;
                }else if(n==0){ //incomplete
                    return 0;
                }else{ //ok
                    _rb.release(n);
                }
                _com->onreceive(&pkt);
                _stage = STAGE::COMMAND;
                return 1;
            }else{ // STAGE::COMMAND
                mysql_request_t req;
                int n = req.decode(&_rb);
                if(n<0){ //failed
                    fprintf(stderr, "Error: request decode failed\n");
                    return -1;
                }else if(n==0){ //incomplete
                    return 0;
                }else{ //ok
                    _rb.release(n);
                }
                _com->onreceive(&req);
                return 1; //1 indicate: continuously receive in evloop
            }
        }else{ //CLIENT SIDE
            mysql_response_t rsp;
            int n = rsp.decode(&_rb);
            if(n<0){ //failed
                fprintf(stderr, "Error: response decode failed\n");
                return -1;
            }else if(n==0){ //incomplete
                return 0;
            }else{ //ok
                _rb.release(n);
            }
            _com->onreceive(&rsp);
            return 1;
        }
    }

    side_type_t _side;
    evloop_t *_ep;
    int _fd;
    comhandler_t *_com;
    STAGE _stage; //1:HANDSHAKE, 3:COMMAND

    buff_t _rb;
    buff_t _wb;
};
