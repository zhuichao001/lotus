#ifndef _NET_CALLBACK_H_
#define _NET_CALLBACK_H_

template<typename REQUEST, typename RESPONSE>
class session_t;

class evloop_t;

//for session, used by dialer
template<typename REQUEST, typename RESPONSE>
using SessionCallback = std::function<int(REQUEST*, RESPONSE*)>;

//for session, used by answer
template<typename REQUEST, typename RESPONSE>
using ProcessCallback = std::function<int(session_t<REQUEST, RESPONSE> *session)>;

using AcceptCallback = std::function<int(evloop_t *rp, int fd)>;

#endif
