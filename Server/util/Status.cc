#include "Status.h"

namespace Mydb {

std::string Status::toString() {
  if (str_ == "" && str_ == "") {
    return "+Ok";
  } else {
    char tmp[30];
    const char* type;
    switch (code()) {
      case kOk:
        type = "+Ok";
        break;
      case kNotFound:
        type = "-NotFound: ";
        break;
      case kCorruption:
        type = "-Corruption: ";
        break;
      case kNotSupported:
        type = "-Not implemented: ";
        break;
      case kInvalidArgument:
        type = "-Invalid argument: ";
        break;
      case kIOError:
        type = "-IO error: ";
        break;
      default:
        snprintf(tmp, sizeof(tmp),
                 "Unknown code(%d): ", static_cast<int>(code()));
        type = tmp;
        break;
    }
    string result(type);
    result.append("\r\n" + str_);
    return result;
  }
}
}  // namespace Mydb
