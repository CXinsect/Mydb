#include "Server.h"

int main (void) {
    EventLoop loop;
    Address listenaddr(6379);
    Server server(&loop,listenaddr);
    loop.loop();
}