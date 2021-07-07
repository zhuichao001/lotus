#include "socket.h"

int bind_address(int fd, const char* ip, int port) {
    struct sockaddr_in servaddr;
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &servaddr.sin_addr);
    servaddr.sin_port = htons(port);

    if (bind(fd,(struct sockaddr*)&servaddr,sizeof(servaddr)) == -1) {
        perror("bind error: ");
        return -1;
    }
    return 0;
}

int set_reuseaddr(int fd, int reuse) {
    if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        return -1;
    }
    return 0;
}

int set_reuseport(int fd, bool on) {
    int opt = on ? 1 : 0;
    int ret = ::setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof opt));
    if (ret < 0 && on) {
        fprintf(stderr, "warning: SO_REUSEPORT failed.\n");
        return -1;
    }
    return 0;
}

int set_unblocking(int fd, int on) {
    int ret = ::ioctl(fd, FIONBIO, (char *)&on);
    if (ret < 0) {
        perror("ioctl() failed");
        ::close(fd);
    }
    return ret;
}

int set_keepalive(int fd, bool on){
    int opt = on ? 1 : 0;
      ::setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE,
                             &opt, static_cast<socklen_t>(sizeof opt));
    return 0;
}

int set_nodelay(int fd, bool on){
    int opt = on ? 1 : 0;
    ::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof opt));
    return 0;
}

int accept(int svrfd){
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int fd = accept(svrfd, (struct sockaddr*)&addr, &addrlen);
    if (fd == -1) {
        if (errno==EINTR || errno==EAGAIN || errno==EPROTO || errno==ECONNABORTED) {
            return 0;
        } else {
            fprintf(stderr, "accept fail,error:%s\n", strerror(errno));
            return -1;
        }
    } 
    return fd;
}

int connect(const char* ip, int port){
    int fd = socket(AF_INET, SOCK_STREAM,0);
    if(fd<0){
        return -1;
    }

    struct sockaddr_in svr_addr;
    svr_addr.sin_family = AF_INET; 
    svr_addr.sin_port = htons(port); 
    if (inet_pton(AF_INET, ip, &svr_addr.sin_addr)<=0){ 
        printf("invalid address or address not supported.\n"); 
        return -1; 
    } 

    if (connect(fd, (struct sockaddr *)&svr_addr, sizeof(svr_addr)) < 0) { 
        printf("connection failed.\n"); 
        return -1; 
    } 

    return fd;
}

int get_tcpinfo(int fd, struct tcp_info *info) {
    socklen_t len = sizeof(*info);
    memset(info, 0, len);
    return ::getsockopt(fd, SOL_TCP, TCP_INFO, info, &len) == 0;
}

bool get_tcpinfo_string(int fd, char* buf, int len) {
    struct tcp_info tcp;
    if (get_tcpinfo(fd, &tcp)) {
        snprintf(buf, len, "unrecovered=%u "
            "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
            "lost=%u retrans=%u rtt=%u rttvar=%u "
            "sshthresh=%u cwnd=%u total_retrans=%u",
            tcp.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
            tcp.tcpi_rto,          // Retransmit timeout in usec
            tcp.tcpi_ato,          // Predicted tick of soft clock in usec
            tcp.tcpi_snd_mss,
            tcp.tcpi_rcv_mss,
            tcp.tcpi_lost,         // Lost packets
            tcp.tcpi_retrans,      // Retransmitted packets out
            tcp.tcpi_rtt,          // Smoothed round trip time in usec
            tcp.tcpi_rttvar,       // Medium deviation
            tcp.tcpi_snd_ssthresh,
            tcp.tcpi_snd_cwnd,
            tcp.tcpi_total_retrans);  // Total retransmits for entire connection
        return true;
    }
    return false;
}
