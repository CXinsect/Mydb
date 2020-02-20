#ifndef SOCKET__H__
#define SOCKET__H__
#include "ModelHead.h"
using namespace std;

class Address;

class Socket {
 public:
  explicit Socket(int sockfd_) : sockfd(sockfd_) {}
  ~Socket();
  int getSockfd(void) { return sockfd; }
  void bindAddress(const Address &localAddr);
  void listen();
  int accept(Address *peerAddress);
  void shutdownWrite();
  void setReusePort(bool on);
  void setReuseAddr(bool on);

 private:
  const int sockfd;
};
#endif