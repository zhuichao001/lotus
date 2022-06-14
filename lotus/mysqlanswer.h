#include "answer.h"
#include "protocol/mysql.h"

template<>
class answer_t<sql_request_t, sql_response_t>:
    public comhandler_t {
public:
    answer_t(evloop_t *ep, int fd, ProcessCallback<sql_request_t, sql_response_t> procb):
        _ep(ep),
        _fd(fd),
        _processcb(procb) {
        get_peer_ip_port(_fd, &(_addr.ip), &(_addr.port));
    }

    ~answer_t(){
        delete _conn;
    }

    int open(){
        _conn = new endpoint_t<sql_request_t, sql_response_t>(endpoint_t<sql_request_t, sql_response_t>::SERVER_SIDE, _ep, _fd, this);
        int err = _conn->open();
        fprintf(stderr, "fd:%d open iohandler\n", _conn->fd());

        send_handshake();
        return err;
    }

    int onclose(){
        delete this;
        return 0;
    }

    int onreceive(void *request){
        if(_conn->stage() == STAGE::HANDSHAKE){
            send_ok();
            return 0;
        }else{ //then STAGE::COMMAND
            sql_request_t *req = static_cast<sql_request_t*>(request);
            fprintf(stderr, "receive mysql request to process\n");
            auto session = new session_t<sql_request_t, sql_response_t>(_conn, req); 
            _processcb(session);
            delete session;
            return 0;
        }
    }

private:
    void send_handshake(){
        handshake_packet_t p;
        buff_t buf;
        p.encode(&buf);
        _conn->send(buf);
    }

    void send_ok(){
        ok_packet_t p;
        buff_t buf;
        p.encode(&buf);
        _conn->send(buf);
    }

private:
    evloop_t *_ep;
    const int _fd;
    address_t _addr;
    timedriver_t *_watcher;
    ProcessCallback<sql_request_t, sql_response_t> _processcb;
    endpoint_t<sql_request_t, sql_response_t> *_conn;
    std::map<uint64_t, session_t<sql_request_t, sql_response_t> *> _sessions;
};
