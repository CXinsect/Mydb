#include "Buffer.h"

void Buffer::prepend(const void *data, size_t len) {
  assert(getPrependableBytes() >= len);
  readIndex_ -= len;
  const char *ptr = static_cast<const char *>(data);
  std::copy(ptr, ptr + len, getDateBegin() + readIndex_);
}
void Buffer::prependInt(int x) {
  int tmp = htonl(x);
  prepend(&tmp, sizeof(tmp));
}
ssize_t Buffer::readFd(int fd) {
  char buf[65535] = {0};
  struct iovec iov[2];
  const size_t writeAble = getWriteableBytes();
  iov[0].iov_base = getDateBegin() + writeIndex_;
  iov[0].iov_len = writeAble;
  iov[1].iov_base = buf;
  iov[1].iov_len = sizeof(buf);
  const size_t n = ::readv(fd, iov, 2);
  std::cout << "n: " << static_cast<int>(n) << ": " << strerror(errno) << "fd"
            << fd << std::endl;

  if (static_cast<int>(n) < 0) {
    std::cout << "Buffer::readFd: " << __LINE__ << std::endl;
  } else if (static_cast<size_t>(n) <= writeAble) {
    writeIndex_ += n;
  } else {
    writeIndex_ = buffer_.size();
    Append(buf, n - writeAble);
  }
  return n;
}
void Buffer::retrieve(size_t len) {
  assert(len <= getReadableBytes());
  if (len < getReadableBytes())
    readIndex_ += len;
  else
    retrieveAllBytes();
}
void Buffer::retrieveUntil(const char *end) {
  assert(peek() <= end);
  assert(end < writeBegin());
  retrieve(end - peek());
}
void Buffer::retrieveAllBytes() {
  readIndex_ = *getDateBegin() + PREPEND;
  writeIndex_ = readIndex_;
}
std::string Buffer::retrieveAsString(size_t len) {
  assert(len <= getReadableBytes());
  std::string ret(peek(), len);
  retrieve(len);
  return ret;
}
void Buffer::hadWriten(size_t len) {
  assert(len <= getWriteableBytes());
  writeIndex_ += len;
}
void Buffer::Append(const char *data, size_t len) {
  if (getWriteableBytes() < len) {
    if (getWriteableBytes() + getPrependableBytes() < len + PREPEND) {
      buffer_.resize(writeIndex_ + len);
    } else {
      assert(PREPEND < readIndex_);
      size_t Read = getReadableBytes();
      // if(Read >= 65535) {
      //     return;
      // }
      std::cout << "readlen: " << Read << std::endl;
      std::copy(getDateBegin() + readIndex_, getDateBegin() + writeIndex_,
                getDateBegin() + PREPEND);
      readIndex_ = PREPEND;
      writeIndex_ = readIndex_ + Read;
    }
  }
  assert(getWriteableBytes() >= len);
  std::copy(data, data + len, writeBegin());
  hadWriten(len);
}