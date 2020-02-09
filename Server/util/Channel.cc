#include "Channel.h"
#include "EventLoop.h"

const int Channel::NoneEvent_ = 0;
const int Channel::ReadEvent_ = EPOLLIN | EPOLLPRI;
const int Channel::WriteEvent_ = EPOLLOUT;
// class EventLoop;

void Channel::update() {  loop_->updateChannel(this); }
void Channel::handleEvent() {
    
    if(revents_ & (EPOLLIN | EPOLLPRI) ) {
         if(readcallback_) readcallback_();
         if(sreadcallback_) sreadcallback_();
    }
       
    if(revents_ & EPOLLOUT)
        if(writecallback_) writecallback_();
}