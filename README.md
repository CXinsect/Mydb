### 介绍
 - Mydb是一款小型的缓存数据库，支持String、Hash、Set、Sorted-Set数据类型的基本操作。

### 运行环境
 ![](https://github.com/CXinsect/Mydb/blob/master/images/hardware.png)
 
### 特点
 - 使用**哈希表、链表、跳跃表**作为数据对象底层的数据结构
 - 使用Epoll多路复用机制管理不同类型的套接字事件，同时通过回调函数作出相应响应
 - 利用智能指针技术管理对象。避免了内存泄露和空闲指针
 - 使用LRU算法、惰性删除和定时删除策略实现对不满足要求的键值对进行管理
 - 使用分散读**readv**，减少了当输入缓冲区满时服务器触发回调的次数，进一步减少了系统调用。
 

### 目录说明
名称|目录
--|:--:
Client|客户端文件
Server|服务器文件
Test|客户端测试文件

### 安装和运行
运行前请安装cmake

```
  cd Server/util
  cmake .
  make
  ./Server
```

###　性能测试
测试代码：
```
#include "../Client/Client.h"
#include <ctime>
int main (int argc,char* argv[]) {
    Client client(6379,"127.0.0.1",600);
    client.Connect();
    clock_t startTime = clock();
    int i;
    while(1) {
        for(i = 0;i < atoi(argv[1]);i++) {
            client.sendRequest("set name test");
        }
        if(i == atoi(argv[1])) break;
    }
    clock_t endTime = clock();
    std::cout << "Time costs: " << (double) (endTime - startTime)/CLOCKS_PER_SEC << std::endl;
}
```
 - 分别执行5000\10000\100000\1000000条命令计算返回的时间
 ![](https://github.com/CXinsect/Mydb/blob/master/images/database.png)

