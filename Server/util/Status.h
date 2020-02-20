#ifndef __STATUS_H_
#define __STATUS_H_

#include <cstring>
#include <string>
using namespace std;

namespace Mydb {

class Status {
 public:
  Status() {
    status_ = kOk;
    str_ = "";
  }
  Status(int status, const string msg) {
    status_ = status;
    str_ = msg;
  }
  ~Status() {}
  static Status Ok() { return Status(); }

  static Status notFound(const string& msg) { return Status(kNotFound, msg); }
  static Status corruption(const string& msg) {
    return Status(kCorruption, msg);
  }
  static Status notSupported(const string& msg) {
    return Status(kNotSupported, msg);
  }
  static Status invalidArgument(const string& msg) {
    return Status(kInvalidArgument, msg);
  }
  static Status ioError(const string& msg) { return Status(kIOError, msg); }
  static Status emptyContent(const string& msg) {
    return Status(kEmptyContent, msg);
  }
  std::string toString();

 public:
  bool OK() { return status_ == kOk; }
  bool isNotFound() const { return status_ == kNotFound; }
  bool isCorruption() const { return status_ == kCorruption; }
  bool isIoError() const { return status_ == kIOError; }
  bool isInVaildArgument() const { return status_ == kInvalidArgument; }
  bool isNotSupported() const { return status_ == kNotSupported; }
  bool isEmptyContent() const { return status_ == kEmptyContent; }

 private:
  enum Code {
    kOk = 0,
    kNotFound = 1,
    kCorruption = 2,
    kNotSupported = 3,
    kInvalidArgument = 4,
    kIOError = 5,
    kEmptyContent = 6
  };
  int status_;
  string str_;
  int code() const { return status_; }
};
}  // namespace Mydb
#endif