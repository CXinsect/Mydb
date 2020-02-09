#ifndef SOCKETOPTS__H__
#define SOCKETOPTS__H__

#include <arpa/inet.h>
#include <string>
namespace sockets {
    int creatNonblocking(sa_family_t family);
    int connect (int sockfd,const struct sockaddr *addr);
    void Bind(int sockfd,const struct sockaddr *addr);
    void Listen(int sockfd);
    int accept(int sockfd,struct sockaddr_in * addr);
    ssize_t read(int sockfd,void *buf,size_t count);
    ssize_t readv(int sockfd,const struct iovec *iov,int);
    ssize_t write(int sockfd,const void *buf,size_t count);
    void close (int sockfd);
    void shutdownWrite (int sockfd);
    void toIpPort (char *buf,size_t size,const struct sockaddr * addr);
    void toIp (char *buf,size_t size,const struct sockaddr * addr);
    void fromIpPort(const char *ip,short port,struct sockaddr_in *addr);
    int getSocketError (int sockfd);
    void myError (const char *err_string,int line);
    struct sockaddr_in getLocalAddr(int sockfd);
    struct sockaddr_in getPeerAddr(int sockfd);
    struct sockaddr* sockaddr_tran(struct sockaddr_in *addr);
    bool isSelfConnection(int sockfd);
}
#endif