#ifndef __STATUS_H_
#define __STATUS_H_

#include <string>
#include <cstring>
using namespace std;

namespace Mydb {

    class Status {
        public:
            Status() {
                status_ = kOk;
                str_ = "";
            }
            Status(int status,const string msg){
                status_ = status;
                str_ = msg;
            }
            ~Status() { }
            static Status Ok() { return Status(); }

            static Status NotFound(const string& msg) {
                return Status(kNotFound,msg);
            }
            static Status Corruption(const string& msg) {
                return Status(kCorruption, msg);
            }
            static Status NotSupported(const string& msg) {
                return Status(kNotSupported, msg);
            }
            static Status InvalidArgument(const string& msg) {
                return Status(kInvalidArgument, msg);
            }
            static Status IOError(const string& msg) {
                return Status(kIOError, msg);
            }
            static Status EmptyContent(const string& msg) {
                return Status(kEmptyContent,msg);
            }
            std::string ToString();
        public:
            bool OK() { return status_ == kOk; }
            bool IsNotFound() const { return status_ == kNotFound; }
            bool IsCorruption() const { return status_ == kCorruption; }
            bool IsIoError() const { return status_ == kIOError; }
            bool IsInVaildArgument() const { return status_ == kInvalidArgument; }
            bool IsNotSupported() const { return status_ == kNotSupported; }
            bool IsEmptyContent() const { return status_ == kEmptyContent; }
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
              int code() const {
                  return status_;
              }
    };
}
#endif