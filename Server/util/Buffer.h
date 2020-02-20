#include "ModelHead.h"

#define PREPEND 8
class Buffer {
 public:
  Buffer()
      : buffer_(1024 + PREPEND), readIndex_(PREPEND), writeIndex_(PREPEND) {
    assert(getReadableBytes() == 0);
    assert(getWriteableBytes() == 1024);
    assert(getPrependableBytes() == PREPEND);
  }
  void append(const char *buf, size_t dateLen);
  size_t getCapicty() const { return buffer_.capacity(); }

  size_t getPrependableBytes() { return readIndex_; };
  void prepend(const void *data, size_t len);
  void prependInt(int len);

  size_t getReadableBytes() const { return writeIndex_ - readIndex_; }
  size_t getWriteableBytes() const { return buffer_.size() - writeIndex_; }
  const char *peek() { return getDateBegin() + readIndex_; }
  void retrieve(size_t len);
  void retrieveUntil(const char *end);
  void retrieveAllBytes();
  std::string retrieveAllAsString() {
    return retrieveAsString(getReadableBytes());
  }
  std::string retrieveAsString(size_t len);
  char *writeBegin() { return getDateBegin() + writeIndex_; }
  void hadWriten(size_t len);
  void Append(const char *data, size_t len);
  ssize_t readFd(int fd);
  int getDateBegin(int) { return static_cast<int>(readIndex_); };

 private:
  char *getDateBegin() { return &*buffer_.begin(); }

 private:
  std::vector<char> buffer_;
  size_t readIndex_;
  size_t writeIndex_;
};