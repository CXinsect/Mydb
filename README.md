### 介绍
 - Mydb是一款小型的缓存数据库，支持String、Hash、Set、Sorted-Set数据类型的基本操作。

### 运行环境
 ![](https://github.com/CXinsect/Mydb/blob/master/images/hardware.png)
 
### 特点
 - 使用**哈希表、链表、跳跃表、红黑树**作为数据对象底层的数据结构
 - 使用单线程Reactor＋非阻塞IO设计，同时通过回调函数作出相应响应
 - 利用智能指针技术管理对象。避免了内存泄露和空闲指针
 - 使用惰性删除策略实现对不满足要求的键值对进行管理
 - 使用分散读**readv**，减少了当输入缓冲区满时服务器触发回调的次数，进一步减少了系统调用。
 - 使用mmap读取文件。提高IO效率
 - 使用LRU算法实现了对内存的管理
 - 使用了扩展的内存分配器，减少了malloc的额外开销
 

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

### 测试

#### 基本功能
```
#include "../Client/Client.h"
#include <ctime>
int main (int argc,char* argv[]) {
    Client client(6379,"127.0.0.1",600);
    client.Connect();
    client.sendRequest("set name test");
    client.sendRequest("get name");
    client.sendRequest("hset tree color black");
    client.sendRequest("hset tree height high");
    client.sendRequest("hget tree");
    client.sendRequest("rpush list didi jiejie");
    client.sendRequest("rpop list");
    client.sendRequest("zadd zset 1 apple 2 cherry");
    client.sendRequest("zcount zset 0 3");
}
```
测试结果
![](https://github.com/CXinsect/Mydb/blob/master/images/component.png)

#### 响应时间
```
#include "../Client/Client.h"
#include <ctime>
int main (int argc,char* argv[]) {
    Client client(6379,"127.0.0.1",600);
    client.Connect();
//set
    clock_t startTime = clock();
    int i;
    while(1) {
        for(i = 0;i < atoi(argv[1]);i++) {
            char buf[10];
            snprintf(buf,3,"t%d",i);
            string t = "set name ";
            t += string(buf);
            client.sendRequest(t);
           
        }
        if(i == atoi(argv[1])) break;
    }
    clock_t endTime = clock();

//get    
    clock_t startTime1 = clock();
    while(1) {
        for(i = 0;i < atoi(argv[1]);i++) {
            client.sendRequest("get name");
           
        }
        if(i == atoi(argv[1])) break;
    }
    clock_t endTime1 = clock();
    std::cout << "set time costs: " << (double) (endTime - startTime)/CLOCKS_PER_SEC << std::endl;
    std::cout << "get time costs: " << (double) (endTime1- startTime1)/CLOCKS_PER_SEC << std::endl;
}
```
### 运行结果
- 分别执行5000\10000\100000\1000000条命令计算返回的时间
  - 执行5000条命令返回的结果
   ![](https://github.com/CXinsect/Mydb/blob/master/images/5000.png)
  - 执行10000条命令返回的结果
   ![](https://github.com/CXinsect/Mydb/blob/master/images/10000.png)
  - 执行100000条命令返回的结果
   ![](https://github.com/CXinsect/Mydb/blob/master/images/100000.png)
  - 执行1000000条命令返回的结果
   ![](https://github.com/CXinsect/Mydb/blob/master/images/1000000.png)
