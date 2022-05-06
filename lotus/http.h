#ifndef _NET_HTTP_H_
#define _NET_HTTP_H_

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <string>

char *strupper(char *data){
    for(int i=0; data[i]!=0 ;++i){
        if(data[i]>='a' && data[i]<='z'){
            data[i] = data[i]-'a'+'A';
        }
    }
    return data;
}

//the length of `to` must be no less than `raw`
int copy_until(const char* &raw, const char *end, char *to, int tolen){
    int i=0; //as index of raw
    int j=0; //as index of end
    int k=0; //as index of to
    while(true){
        if(raw[i]!=end[j]){
            for(int m=0; m<j; ++m){
                if(k==tolen-1){
                    return -1;
                }
                to[k++] = end[m];
            }
            if(k==tolen-1){
                return -1;
            }
            to[k++] = raw[i++];
            j=0;
        }else{
            ++i;
            ++j;
            if(end[j]==0){
                break;
            }
            if(raw[i]==0){
                return -1;
            }
        }
    }
    raw = raw+i;
    return 0;
}

const char * http_version = "HTTP/1.1";

typedef std::string mime_t;
const mime_t TEXT_HTML  = "text/html";
const mime_t TEXT_PLAIN = "text/plain";
const mime_t TEXT_XML   = "text/xml";
const mime_t IMG_PNG    = "image/png";
const mime_t APP_JSON   = "application/json";
const mime_t APP_FORM   = "x-www-form-urlencoded";

typedef std::string method_t;
const method_t GET      = "GET";
const method_t POST     = "POST";
const method_t PUT      = "PUT";
const method_t DELETE   = "DELETE";
const method_t HEAD     = "HEAD";

class http_request_t {
    method_t    _method;
    std::string _host;
    std::string _uri;
    std::string _accept;
    std::string _body;
    bool        _keep_alive;
public:
    http_request_t(const method_t &m, const std::string &host, const std::string &uri, const mime_t &accept=TEXT_PLAIN, const std::string &body="", bool alive=false):
        _method(m),
        _host(host),
        _uri(uri),
        _accept(accept),
        _body(body),
        _keep_alive(alive){
    }

    int encode(char *buf, int len){
        assert(buf!=nullptr);
        sprintf(buf, "%s %s %s\r\n \
                Host:%s\r\n \
                Connection:%s\r\n \
                Accept:%s\r\n \
                Accept-Charset:utf-8\r\n\0",
                _method, _uri.c_str(), http_version, 
                _host.c_str(),
                _keep_alive?"KEEP-ALIVE":"CLOSE",
                _accept);
        const int buf_len = strlen(buf);
        if(_body.size()>0){
            sprintf(buf+buf_len, "Content-Type:%s\r\n \
                    Content-Length:%d\r\n \
                    %s\r\n\0", 
                    APP_JSON, 
                    _body.size(),
                    _body.c_str());
        }else{
            sprintf(buf+buf_len, "\r\n\0");
        }
        return 0;
    }

    int decode(const char *buf, int len){
        char tmp[512];
        const char *raw = buf;
        if(copy_until(raw, " ", tmp, sizeof(tmp))<0){
            return -1;
        }
        _method = strupper(tmp);

        if(copy_until(raw, " ", tmp, sizeof(tmp))<0){
            return -1;
        }
        _uri = tmp;

        if(copy_until(raw, "\r\n", tmp, sizeof(tmp))<0){
            return -1;
        }
        //ignore

        while(true){
            if(copy_until(raw, ":", tmp, sizeof(tmp))<0){
                return -1;
            }
            if(strlen(tmp)==0){
                break;
            }

            char key[64];
            char val[256];
            const char *tmp_raw = tmp;
            copy_until(tmp_raw, ":", key, sizeof(tmp));
            copy_until(tmp_raw, "\r\n", val, sizeof(tmp));

            if(strcasecmp(key, "Host")==0){
                _host = val;
            }else if(strcasecmp(key, "Connection")==0){
                if(strcasecmp(val, "Keep-Alive")==0){
                    _keep_alive = true;
                }else{
                    _keep_alive = false;
                }
            }else if(strcasecmp(key, "Accept")==0){
                _accept = strupper(val);
            }
        }
        copy_until(raw, "\0", tmp, sizeof(tmp));
        _body = tmp;
        return 0;
    }
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

const char *status_message(status_t &status){
    switch(status){
        case OK:
            return "OK";    
        case BAD_REQUEST:
            return "Bad Request";    
        case UNAUTHORIZED:
            return "Unauthorized";    
        case FORBIDDEN:
            return "Forbidden";    
        case NOT_FOUND:
            return "Not Found";    
        case INTERNAL_SERVER_ERR:
            return "Internal Server Error";    
        case SERVER_UNAVAILABLE:
            return "Server Unavailable";    
        default:
            return "Unknown Status Code";
    }
}

class http_response_t {
    status_t _status;
    mime_t _content_type;
    int _content_len;
    std::string _body;
public:
    http_response_t(const status_t &status, const mime_t &content_type=TEXT_PLAIN, const std::string &body=""):
        _status(status),
        _content_type(content_type),
        _body(body) {
        _content_len = _body.size();
    }

    int encode(char *buf, int len){
        assert(buf!=nullptr);
        sprintf(buf, "%s %d %s\r\n \
                Content-Type:%s\r\n \
                Content-Length:%d\r\n \
                \r\n \
                %s\0",
                http_version, _status, status_message(_status), 
                _content_type,
                _body.size(),
                _body.c_str());
        return 0;
    }

    int decode(const char *buf, int len){
        char tmp[256];
        const char *raw = buf;
        if(copy_until(raw, " ", tmp, sizeof(tmp))<0){
            return -1;
        }
        //ignore http version

        if(copy_until(raw, " ", tmp, sizeof(tmp))<0){
            return -1;
        }
        _status = static_cast<status_t>(atoi(tmp));

        if(copy_until(raw, "\r\n", tmp, sizeof(tmp))<0){
            return -1;
        }
        //ignore status message

        while(true){
            if(copy_until(raw, "\r\n", tmp, sizeof(tmp))<0){
                return -1;
            }
            if(strlen(tmp)==0){
                break;
            }

            char key[64];
            char val[256];
            const char *tmp_raw = tmp;
            copy_until(tmp_raw, ":", key, sizeof(tmp));
            copy_until(tmp_raw, "\r\n", val, sizeof(tmp));

            if(strcasecmp(key, "Content-Type")==0){
                _content_type = val;
            }else if(strcasecmp(key, "Content-Length")==0){
                _content_len = atoi(val);
            }
        }
        copy_until(raw, "\0", tmp, sizeof(tmp));
        _body = tmp;

        if(_body.size()==_content_len){
            return 0;
        }else{
            return 1;
        }
    }
};

#endif
