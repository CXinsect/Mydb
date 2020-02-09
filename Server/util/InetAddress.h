#ifndef ADDRESS__H__
#define ADDRESS__H__

#include <sys/socket.h>
#include <string>
#include <netinet/in.h>

class Address {
    public:
        explicit Address (short port = 0,bool loopbackOnly = false);
        Address (std::string &ip,short port);
        explicit Address (const struct sockaddr_in &addr) : 
                        addr_(addr) {}
        //  sa_family_t family () const { return addr_.sin_family; }
        const struct sockaddr* getSockAddr () const {
            const struct sockaddr * addr4 = static_cast <const struct sockaddr*>(static_cast<const void*>(&addr_));
            return addr4;
        }
        std::string toIp () const;
        std::string toIpPort() const;
        short toPort() const;

    private:
         struct sockaddr_in addr_;
};
#endif