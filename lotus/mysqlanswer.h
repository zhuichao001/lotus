#include "answer.h"

template<>
class answer_t<sql_request_t, sql_response_t>:
    public comhandler_t {
public:
    answer_t(evloop_t *ep, int fd, ProcessCallback<sql_request_t, sql_response_t> procb):
        _ep(ep),
        _fd(fd),
        _processcb(procb),
        _stage(-1){
        get_peer_ip_port(_fd, &(_addr.ip), &(_addr.port));
    }

    ~answer_t(){
        for(auto it : _sessions){
            int msgid = it.first;
            delete _sessions[msgid];
        }
        delete _conn;
    }

    int open(){
        _conn = new endpoint_t<sql_request_t, sql_response_t>(endpoint_t<sql_request_t, sql_response_t>::SERVER_SIDE, _ep, _fd, this);
        int err = _conn->open();
        fprintf(stderr, "fd:%d open iohandler\n", _conn->fd());

        send_handshake();
        _stage = 1;
        return err;
    }

    int onclose(){
        delete this;
        return 0;
    }

    int onreceive(void *request){
        if(_stage == 1){
            recv_auth();
            //TODO: SEND ok packet

            return 0;
        }else if(_stage == 0){
            sql_request_t *req = static_cast<sql_request_t*>(request);
            uint64_t msgid = req->msgid();
            fprintf(stderr, "receive msg:%d to process\n", msgid);
            _sessions[msgid] = new session_t<sql_request_t, sql_response_t>(_conn, req); 
            _processcb(_sessions[msgid]);

            delete _sessions[msgid];
            _sessions.erase(msgid);
        }else{
            fprintf(stderr, "unknown stage:%d\n", _stage);
        }
        return 0;
    }

private:
    void send_handshake(){
        handshake_packet_t p;
        buff_t buf;
        p.encode(&buf);
        _conn->send(buf);
        _state = 0;
    }

private:
    evloop_t *_ep;
    const int _fd;
    address_t _addr;
    timedriver_t *_watcher;
    ProcessCallback<sql_request_t, sql_response_t> _processcb;
    endpoint_t<sql_request_t, sql_response_t> *_conn;
    std::map<uint64_t, session_t<sql_request_t, sql_response_t> *> _sessions;

    int _stage; //1:HANDSHAKE, 0:COMMAND
};
