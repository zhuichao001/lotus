#include "http.h"

int main(){
    http_request_t req("GET", "www.baidu.com", "/page/api", APP_JSON, "a=1&b=2", true);

    http_response_t rsp(OK, APP_JSON, "{\"version\":1}");
    return 0;
}
