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
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        return -1;
    }
    return 0;
}

int set_unblocking(int fd, int on) {
    int ret = ioctl(fd, FIONBIO, (char *)&on);
    if (ret < 0) {
        perror("ioctl() failed");
        close(fd);
    }
    return ret;
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
