#include <sys/timerfd.h>
#include <iostream>
#include "../Server/EventLoop.cc"
#include "../Server/Channel.cc"

EventLoop *tloop;

void timeout() {
    std::cout << "timeout" << std::endl;
    tloop->quit();
}

int main (void) {
    EventLoop loop;
    tloop = &loop;
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);
    Channel channel(&loop,timerfd);
    channel.setReadCallback(timeout);
    channel.enableReading();
    struct itimerspec howlong;
    memset(&howlong,0,sizeof(howlong));
    howlong.it_value.tv_sec = 3;
    ::timerfd_settime(timerfd,0,&howlong,NULL);
    loop.loop();
    ::close(timerfd);

}