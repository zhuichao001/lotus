#include "answer.h"
#include "protocol/mysql.h"

template<>
class answer_t<mysql_request_t, mysql_response_t>:
    public comhandler_t {
public:
    answer_t(evloop_t *ep, int fd, ProcessCallback<mysql_request_t, mysql_response_t> procb):
        _ep(ep),
        _fd(fd),
        _stage(STAGE::HANDSHAKE),
        _processcb(procb) {
        get_peer_ip_port(_fd, &(_addr.ip), &(_addr.port));
    }

    ~answer_t(){
        delete _conn;
    }

    int open(){
        _conn = new endpoint_t(_ep, _fd, this);
        int err = _conn->open();
        fprintf(stderr, "fd:%d open iohandler\n", _conn->fd());

        send_handshake();
        return err;
    }

    int onclose()override{
        delete this;
        return 0;
    }

    int onreceive(buff_t *buf)override{
        fprintf(stderr, "onreceive:\n");
        buf->print();
        if(_stage == STAGE::HANDSHAKE){
            handshake_response_t pkt;
            int n = pkt.decode(buf);
            if(n<0){ //failed
                fprintf(stderr, "Error: request decode failed\n");
                return -1;
            }else if(n==0){ //incomplete
                return 0;
            }else{ //ok
                buf->release(n);
            }
            send_ok();
            _stage = STAGE::COMMAND;
            return 1;
        }else{ //then STAGE::COMMAND
            mysql_request_t req;
            int n = req.decode(buf);
            if(n<0){ //failed
                fprintf(stderr, "Error: request decode failed\n");
                return -1;
            }else if(n==0){ //incomplete
                return 0;
            }else{ //ok
                buf->release(n);
            }

            fprintf(stderr, "receive mysql request to process\n");
            auto session = new session_t<mysql_request_t, mysql_response_t>(_conn, &req); 
            _processcb(session);
            delete session;
            return 1; //1 indicate: continuously receive in evloop
        }
    }

private:
    void send_handshake(){
        handshake_request_t p;
        buff_t buf;
        p.encode(&buf);
        fprintf(stderr, "send handshake request:\n");
        buf.print();
        _conn->send(&buf);
    }

    void send_ok(){
        ok_packet_t p;
        buff_t buf;
        p.encode(&buf);
        _conn->send(&buf);
    }

private:
    evloop_t *_ep;
    const int _fd;
    address_t _addr;
    timedriver_t *_watcher;
    ProcessCallback<mysql_request_t, mysql_response_t> _processcb;
    endpoint_t *_conn;
    std::map<uint64_t, session_t<mysql_request_t, mysql_response_t> *> _sessions;
    STAGE _stage; //1:HANDSHAKE, 3:COMMAND
};
