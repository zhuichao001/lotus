#pragma once

#include "engine.h"
#include "service.h"
#include "dialer.h"
#include "answer.h"
#include "session.h"
#include "protocol/rpc.h"
#include "protocol/http.h"

using rpc_session_t = session_t<rpc_request_t, rpc_response_t>;

using rpc_service_t = service_t<rpc_request_t, rpc_response_t>;
