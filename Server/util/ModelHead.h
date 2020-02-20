#ifndef _MODEL_H_
#define _MODEL_H_
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace ::std::placeholders;
using namespace std;

namespace dataStructure {
//对象
const short ObjString = 0;
const short ObjHash = 1;
const short ObjList = 2;
const short ObjZset = 3;
//编码
const int EncodingString = 0;
const int EncodingDict = 1;
const int EncodingLinkedList = 2;
const int EncodingSkipList = 3;
//事件属性
const int NoEvent = 0;
const int ReadEvent = 1;
const int WriteEvent = 2;

//网络配置
const int Port = 8888;
const std::string Ip = "127.0.0.1";
const int saveTime = 1;
const std::string SpareTire = "SPACE";

}  // namespace DataStructure

const long long DefaultTime = 2038;

typedef std::function<void(std::vector<std::string> &)> callback;

class Accept;
typedef std::shared_ptr<Accept> AcceptorPtr;
#endif