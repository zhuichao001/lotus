#ifndef _NET_CALLBACK_H_
#define _NET_CALLBACK_H_

#include <functional>


//for session, used by dialer
template<typename REQUEST, typename RESPONSE>
using SessionCallback = std::function<int(REQUEST*, RESPONSE*)>;

template<typename REQUEST, typename RESPONSE>
class session_t;
//for session, used by answer
template<typename REQUEST, typename RESPONSE>
using ProcessCallback = std::function<int(session_t<REQUEST, RESPONSE> *session)>;

class evloop_t;
using AcceptCallback = std::function<int(evloop_t *rp, int fd)>;

#endif
