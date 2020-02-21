#include "Client.h"
size_t Io::writen(int sockfd, char *buf, ssize_t count) {
  assert(sockfd != -1);
  assert(buf != NULL);
  assert(count != 0);
  char *bufp = (char *)buf;
  size_t nleft = count;
  int nwrite = 0;
  while (nleft > 0) {
    nwrite = write(sockfd, bufp, nleft);
    if (nwrite < 0) {
      if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) continue;
      return -1;
    }
    if (nwrite == 0) continue;
    nleft -= nwrite;
    bufp += nwrite;
  }
  return count - nleft;
}
size_t Io::readn(int sockfd, char *buf, ssize_t size) {
  assert(sockfd != -1);
  assert(buf != NULL);
  assert(size != 0);
flags:
  char *bufp = buf;
  int nread = 0;
  int count = 0;
  while (1) {
    nread = read(sockfd, bufp, size);
    if (nread < 0) {
      if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) {
        break;
      }
      return -1;
    } else if (nread == 0) {
      std::cout << "Server is Over" << std::endl;
      return count;
    }
    count += nread;
  }
  if (count == 0) goto flags;
  return count;
}
int Client::setNoOrBlocking(int sockfd) {
  int ret = ::fcntl(sockfd, F_GETFL);
  assert(ret != -1);
  int flags = ret | O_NONBLOCK;
  int res = ::fcntl(sockfd, F_SETFL, flags);
  assert(res != -1);
  return ret;
}
int Client::Connect() {
  int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
  assert(sockfd != -1);
  struct sockaddr_in serv;
  memset(&serv, 0, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_port = htons(port_);
  serv.sin_addr.s_addr = ::inet_addr(Ip_.c_str());
  int original = setNoOrBlocking(sockfd);
  int ret = ::connect(sockfd, (struct sockaddr *)&serv, sizeof(serv));
  // std::cout << strerror(errno) << std::endl;
  confd_ = sockfd;
  if (ret < 0 && errno != EINPROGRESS) {
    ::close(confd_);
    return -1;
  } else if (ret == 0) {
    int res = ::fcntl(confd_, F_SETFL, original);
    assert(res != -1);
    return confd_;
  }

  fd_set wd;
  FD_ZERO(&wd);
  FD_SET(confd_, &wd);
  struct timeval tv;
  tv.tv_sec = (time_t)time_;
  tv.tv_usec = 0;
  ret = select(confd_+1, NULL, &wd, NULL, &tv);
  if (ret <= 0) {
    std::cout << "Connect timeout" << std::endl;
    if(errno == EINVAL) cout << "time invaild" << tv.tv_sec<< endl;
    ::close(confd_);
    return -1;
  } else if (!FD_ISSET(confd_, &wd)) {
    std::cout << "No Active Event" << std::endl;
    ::close(confd_);
    return -1;
  }
  int err;
  socklen_t len = sizeof(err);
  ret = ::getsockopt(confd_, SOL_SOCKET, SO_ERROR, &err, &len);
  assert(ret >= 0);
  if (err != 0) {
    std::cout << "Socket Wrong" << std::endl;
    ::close(confd_);
    return -1;
  }
  ret = ::fcntl(confd_, F_SETFL, original);
  assert(ret != -1);
  return confd_;
}
void Client::sendRequest(const std::string &buf) {
  std::istringstream str(buf);
  std::string cmd, key, value;
  char buffer[1024] = {0};
  str >> cmd;
  assert(cmd.c_str() != NULL);
  if (cmd == "get") {
    str >> key;
    assert(key.c_str() != NULL);
    assert(confd_ != -1);
    snprintf(buffer, sizeof(buffer), "*2\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n",
             (int)cmd.size(), cmd.c_str(), (int)key.size(), key.c_str());
    AuxiliaryFun(buffer);
  } else if (cmd == "set") {
    str >> key;
    assert(key.c_str() != NULL);
    str >> value;
    assert(value.c_str() != NULL);
    snprintf(buffer, sizeof(buffer),
             "*3\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n", (int)cmd.size(),
             cmd.c_str(), (int)key.size(), key.c_str(), (int)value.size(),
             value.c_str());
    AuxiliaryFun(buffer);
  } else if (cmd == "bgsave") {
    snprintf(buffer, sizeof(buffer), "*1\r\n$%d\r\n%s\r\n", (int)cmd.size(),
             cmd.c_str());
    AuxiliaryFun(buffer);

  } else if (cmd == "del") {
    str >> key;
    assert(key.c_str() != NULL);
    snprintf(buffer, sizeof(buffer), "*2\r\n$%d\r\n%s$\r\n$%d\r\n%s\r\n",
             (int)cmd.size(), cmd.c_str(), (int)key.size(), key.c_str());
    AuxiliaryFun(buffer);

  } else if (cmd == "select") {
    str >> key;
    assert(key.c_str() != NULL);
    str >> value;
    assert(value.c_str() != NULL);
    snprintf(buffer, sizeof(buffer), "*2\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n",
             (int)cmd.size(), cmd.c_str(), (int)key.size(), key.c_str());
    AuxiliaryFun(buffer);
  } else if (cmd == "expire") {
    str >> key;
    assert(key.c_str() != NULL);
    str >> value;
    assert(value.c_str() != NULL);
    snprintf(buffer, sizeof(buffer),
             "*3\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n", (int)cmd.size(),
             cmd.c_str(), (int)key.size(), key.c_str(), (int)value.size(),
             value.c_str());
    AuxiliaryFun(buffer);
  } else if (cmd == "hset") {
    str >> key;
    assert(key.c_str() != NULL);
    std::string key1;
    str >> key1;
    str >> value;
    snprintf(buffer, sizeof(buffer),
             "*4\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n",
             (int)cmd.size(), cmd.c_str(), (int)key.size(), key.c_str(),
             (int)key1.size(), key1.c_str(), (int)value.size(), value.c_str());
    AuxiliaryFun(buffer);
  } else if (cmd == "hget") {
    str >> key;
    snprintf(buffer, sizeof(buffer), "*2\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n",
             (int)cmd.size(), cmd.c_str(), (int)key.size(), key.c_str());
    AuxiliaryFun(buffer);
  } else if (cmd == "hgetall") {
    str >> key;
    assert(key.c_str() != NULL);
    snprintf(buffer, sizeof(buffer), "*1\r\n$%d\r\n%s\r\n", (int)cmd.size(),
             cmd.c_str());
    AuxiliaryFun(buffer);
  } else if (cmd == "rpush") {
    str >> key;
    assert(key.c_str() != NULL);
    std::string value;
    char _buf[1024] = {0};
    int num = 0, len = 0;
    int count = 0;
    while (str >> value) {
      count++;
      num = snprintf(_buf + len, sizeof(_buf) - len, "%s ", value.c_str());
      len += num;
    }
    snprintf(buffer, sizeof(buffer),
             "*%d\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n", count,
             (int)cmd.size(), cmd.c_str(), (int)key.size(), key.c_str(),
             static_cast<int>(strlen(_buf)), _buf);
    AuxiliaryFun(buffer);

  } else if (cmd == "rpop") {
    str >> key;
    assert(key.c_str() != NULL);
    snprintf(buffer, sizeof(buffer), "*2\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n",
             (int)cmd.size(), cmd.c_str(), (int)key.size(), key.c_str());
    AuxiliaryFun(buffer);
  } else if (cmd == "zadd") {
    str >> key;
    assert(key.c_str() != NULL);
    string key1, value;
    str >> key1;
    str >> value;
    snprintf(buffer, sizeof(buffer),
             "*4\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n",
             (int)cmd.size(), cmd.c_str(), (int)key.size(), key.c_str(),
             (int)key1.size(), key1.c_str(), (int)value.size(), value.c_str());
    AuxiliaryFun(buffer);
  } else if (cmd == "zcount") {
    str >> key;
    assert(key.c_str() != NULL);
    string lo, hi;
    str >> lo;
    str >> hi;
    snprintf(buffer, sizeof(buffer),
             "*4\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n$%d\r\n%s\r\n",
             (int)cmd.size(), cmd.c_str(), (int)key.size(), key.c_str(),
             (int)lo.size(), lo.c_str(), (int)hi.size(), hi.c_str());
    AuxiliaryFun(buffer);        
  } else {
    std::cout << "The command entered is incorrect, please re-enter"
              << std::endl;
    return;
  }
}
