#include "SocketOpts.h"

#include "ModelHead.h"

void sockets::myError(const char *err_string, int line) {
  std::cout << __func__ << ": " << line << std::endl;
  perror(err_string);
  exit(1);
}
int sockets::creatNonblocking(sa_family_t family) {
  int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
  std::cout << sockfd << std::endl;
  if (sockfd < 0) {
    myError("socket", __LINE__);
  }
  return sockfd;
}
void sockets::Bind(int sockfd, const struct sockaddr *addr) {
  int ret = ::bind(sockfd, addr, sizeof(struct sockaddr));
  if (ret < 0) myError("bind", __LINE__);
}
void sockets::Listen(int sockfd) {
  int ret = ::listen(sockfd, SOMAXCONN);
  if (ret < 0) myError("listen", __LINE__);
}
int sockets::accept(int sockfd, struct sockaddr_in *addr) {
  socklen_t addrlen = static_cast<socklen_t>(sizeof(*addr));
  struct sockaddr *addr4 =
      static_cast<struct sockaddr *>(static_cast<void *>(addr));
  int confd = ::accept4(sockfd, addr4, &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
  if (confd < 0) {
    std::cout << "[Accept Err]"
              << " " << __LINE__ << std::endl;
    int Errno = errno;
    switch (Errno) {
      case EAGAIN:
      case ECONNABORTED:
      case EINTR:
      case EPERM:
      case EMFILE:
        errno = Errno;
        break;
      case EINVAL:
      case ENOMEM:
      default:
        std::cout << "Unknown Errer of accept" << std::endl;
        break;
    }
  }
  return confd;
}
int sockets::connect(int sockfd, const struct sockaddr *addr) {
  return ::connect(sockfd, addr,
                   static_cast<socklen_t>(sizeof(struct sockaddr)));
}
ssize_t sockets::read(int sockfd, void *buf, size_t count) {
  return ::read(sockfd, buf, count);
}
ssize_t sockets::readv(int sockfd, const struct iovec *iov, int cnt) {
  return ::readv(sockfd, iov, cnt);
}
ssize_t sockets::write(int sockfd, const void *buf, size_t count) {
  return ::write(sockfd, buf, count);
}
void sockets::close(int sockfd) {
  if (::close(sockfd) < 0) myError("close", __LINE__);
}
void sockets::shutdownWrite(int sockfd) {
  if (::shutdown(sockfd, SHUT_WR) < 0) myError("shutdownWrite", __LINE__);
}
void sockets::toIpPort(char *buf, size_t size, const struct sockaddr *addr) {
  toIp(buf, size, addr);
  size_t end = ::strlen(buf);
  const struct sockaddr_in *addr1 =
      static_cast<const struct sockaddr_in *>(static_cast<const void *>(addr));
  short port = ntohs(addr1->sin_port);
  //判断缓冲区大是否足够
  assert(size > end);
  snprintf(buf + end, size - end, ":%u", port);
}

//打印ip
void sockets::toIp(char *buf, size_t size, const struct sockaddr *addr) {
  assert(size >= INET_ADDRSTRLEN);
  const struct sockaddr_in *addr1 =
      static_cast<const struct sockaddr_in *>(static_cast<const void *>(addr));
  ::inet_ntop(AF_INET, &addr1->sin_addr, buf, static_cast<socklen_t>(size));
}
void sockets::fromIpPort(const char *ip, short port, struct sockaddr_in *addr) {
  addr->sin_family = AF_INET;
  addr->sin_port = htons(port);
  if (inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
    myError("inet_pton", __LINE__);
}

int sockets::getSocketError(int sockfd) {
  int optval;
  socklen_t len = static_cast<socklen_t>(sizeof(optval));
  if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &len) < 0)
    return errno;
  else
    return optval;
}
struct sockaddr *sockets::sockaddr_tran(struct sockaddr_in *addr) {
  struct sockaddr *addr_ =
      static_cast<struct sockaddr *>(static_cast<void *>(addr));
  return addr_;
}
struct sockaddr_in sockets::getLocalAddr(int sockfd) {
  struct sockaddr_in localaddr;
  memset(&localaddr, 0, sizeof(localaddr));
  socklen_t addrlen = static_cast<socklen_t>(sizeof(localaddr));
  if (::getsockname(sockfd, sockaddr_tran(&localaddr), &addrlen) < 0) {
    myError("getsockname", __LINE__);
  }
  return localaddr;
}
struct sockaddr_in sockets::getPeerAddr(int sockfd) {
  struct sockaddr_in peerAddr;
  memset(&peerAddr, 0, sizeof(peerAddr));
  socklen_t addrlen = static_cast<socklen_t>(sizeof(peerAddr));
  if (::getpeername(sockfd, sockaddr_tran(&peerAddr), &addrlen) < 0)
    myError("getsockname", __LINE__);
  return peerAddr;
}
