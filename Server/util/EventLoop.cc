#include "EventLoop.h"

void EventLoop::loop() {
    assert(!looping_);
    looping_ = true;
    quit_ = false;
    while(!quit_) {
        activeChannels_.clear();
        
        epoll_->epoll(&activeChannels_);
        for(ChannelList::iterator it = activeChannels_.begin();
                    it != activeChannels_.end();it++)
        {
            (*it)->handleEvent();
        }
    }
    looping_ = false;
}
void EventLoop::updateChannel(Channel *channel) {
    epoll_->updateChannel(channel);
}
