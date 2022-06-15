#pragma once

#include "engine.h"
#include "service.h"
#include "dialer.h"
#include "answer.h"
#include "mysqlanswer.h"
#include "session.h"
#include "protocol/rpc.h"
#include "protocol/http.h"
#include "protocol/mysql.h"

using rpc_session_t = session_t<rpc_request_t, rpc_response_t>;
using rpc_service_t = service_t<rpc_request_t, rpc_response_t>;

using http_session_t = session_t<http_request_t, http_response_t>;
using http_service_t = service_t<http_request_t, http_response_t>;

using mysql_session_t = session_t<mysql_request_t, mysql_response_t>;
using mysql_service_t = service_t<mysql_request_t, mysql_response_t>;
