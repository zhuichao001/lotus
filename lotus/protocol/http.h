#pragma once

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include "message.h"
#include "../buff.h"


extern const char * http_version;

typedef std::string mime_t;
extern const mime_t TEXT_HTML; 
extern const mime_t TEXT_PLAIN; 
extern const mime_t TEXT_XML;
extern const mime_t IMG_PNG;  
extern const mime_t APP_JSON;  
extern const mime_t APP_FORM;   

typedef std::string method_t;
extern const method_t GET;
extern const method_t POST;
extern const method_t PUT;
extern const method_t DELETE;
extern const method_t HEAD;

class http_request_t :
    public message_t{
    method_t    _method;
    std::string _host;
    std::string _uri;
    std::string _accept;
    std::string _body;
    bool        _keep_alive;

    static uint64_t BASE_MSGID;
public:
    http_request_t():
        message_t(TYPE_REQUEST){
        setmsgid(++http_request_t::BASE_MSGID);
    }

    http_request_t(const method_t &m, const std::string &host, const std::string &uri, const mime_t &accept=TEXT_PLAIN, const std::string &body="", bool alive=false):
        message_t(TYPE_REQUEST),
        _method(m),
        _host(host),
        _uri(uri),
        _accept(accept),
        _body(body),
        _keep_alive(alive){
        setmsgid(++http_request_t::BASE_MSGID);
    }

    int encode(buff_t *to);

    int decode(buff_t *from);
};

enum status_t {
    OK              = 200,
    BAD_REQUEST     = 400,
    UNAUTHORIZED    = 401,
    FORBIDDEN       = 403,
    NOT_FOUND       = 404,
    INTERNAL_SERVER_ERR = 500,
    SERVER_UNAVAILABLE  = 503,
};

const char *status_message(status_t &status);

class http_response_t :
    public message_t{
    status_t _status;
    mime_t _content_type;
    int _content_len;
    std::string _body;
public:
    http_response_t():
        message_t(TYPE_RESPONSE),
        _status(OK),
        _content_type(TEXT_PLAIN),
        _content_len(0) {
    }

    http_response_t(const status_t &status, const std::string &body="", const mime_t &content_type=TEXT_PLAIN):
        message_t(TYPE_RESPONSE),
        _status(status),
        _content_type(content_type),
        _body(body) {
        _content_len = _body.size();
    }

    void set_response(const status_t &status, const std::string &body="", const mime_t &content_type=TEXT_PLAIN){
        _status = status;
        _body = body;
        _content_len = _body.size();
        _content_type = content_type;
    }

    int encode(buff_t *to);

    int decode(buff_t *from);
};
