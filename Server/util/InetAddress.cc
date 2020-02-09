#include "InetAddress.h"
#include "modelHead.h"
#include "SocketOpts.h"

static const in_addr_t InaddrAny = INADDR_ANY;
static const in_addr_t InaddrLoopback = INADDR_LOOPBACK;

Address::Address(short port,bool loopback) {
    memset(&addr_,0,sizeof(addr_));
    addr_.sin_family = AF_INET;
    in_addr_t ip = loopback ? InaddrLoopback : InaddrAny;
    addr_.sin_addr.s_addr = htonl(ip);
    addr_.sin_port = htons(port);
}
Address::Address(std::string &ip,short port) {
    memset(&addr_,0,sizeof(addr_));
    sockets::fromIpPort(ip.c_str(),port,&addr_);
}

std::string Address::toIpPort() const {
    char buf[64] = "";
    sockets::toIpPort(buf,sizeof(buf),getSockAddr());
    return buf;
}
std::string Address::toIp() const {
    char buf[64] = "";
    sockets::toIp(buf,sizeof(buf),getSockAddr());
    return buf;
}
