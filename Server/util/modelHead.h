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
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <unistd.h>
#include <time.h>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>

using namespace ::std::placeholders;
using namespace std;

namespace DataStructure
{
//对象
const int ObjString = 0;
const int ObjHash = 1;
const int ObjList = 2;
const int ObjZset = 3;
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
const int saveTime = 60 * 60;
const std::string SpareTire = "SPACE";

} // namespace DataStructure

const long long DefaultTime = 2038;

typedef std::function<void(std::vector<std::string> &)> callback;

class Accept;
typedef std::shared_ptr<Accept> AcceptorPtr;
#endif