#include <iostream>
#include "model.h"
#include "../Server/EventLoop.cc"
#include "../Server/Channel.cc"
#include "../Server/Accept.cc"

void onConnection(const AcceptorPtr& conn) {
    std::cout << "New Connection: " << conn->getSocketInfo() << std::endl;
    // conn->send("hello,linux");
}
void onMessage(const AcceptorPtr& conn,Buffer *buf,ssize_t len) {
    std::cout << "Message: " << (int)len << std::endl;
    std::string message = "hello,linux";
    // conn->send(message);
}
int main (void) {
    Address listenaddr(8888);
    EventLoop loop;
    // Accept accept(&loop,listenaddr);
    std::shared_ptr<Accept> accept(new Accept(&loop,listenaddr));
    accept->setConnectionCallBack(onConnection);
    accept->setMessageCallBack(onMessage);
    accept->listen();
    loop.loop();
    return 0;
}