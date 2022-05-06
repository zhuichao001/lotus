#include "util.h"
#include "http.h"

const char * http_version = "HTTP/1.1";

const mime_t TEXT_HTML  = "text/html";
const mime_t TEXT_PLAIN = "text/plain";
const mime_t TEXT_XML   = "text/xml";
const mime_t IMG_PNG    = "image/png";
const mime_t APP_JSON   = "application/json";
const mime_t APP_FORM   = "x-www-form-urlencoded";

const method_t GET      = "GET";
const method_t POST     = "POST";
const method_t PUT      = "PUT";
const method_t DELETE   = "DELETE";
const method_t HEAD     = "HEAD";

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

int http_request_t::encode(char *buf, int len){
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

int http_request_t::decode(const char *buf, int len){
    char tmp[512];
    const char *raw = buf;
    if(copy_until(raw, " ", tmp, sizeof(tmp))<0){
        return -1;
    }
    _method = str2upper(tmp);

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
            _accept = str2upper(val);
        }
    }
    copy_until(raw, "\0", tmp, sizeof(tmp));
    _body = tmp;
    return 0;
}

int http_response_t::encode(char *buf, int len){
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

int http_response_t::decode(const char *buf, int len){
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
