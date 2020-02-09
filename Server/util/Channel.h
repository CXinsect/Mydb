#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include "modelHead.h"
class EventLoop;

class Channel {
    public:
        Channel() {}
        Channel(EventLoop*loop, const int &sockfd):
                        loop_(loop),
                        sockfd_(sockfd),
                        events_(0),
                        revents_(0),
                        index_(-1) {}
        typedef std::function <void()>  EventCallBack;
        void  handleEvent();
        void  setReadCallback(const EventCallBack &cb)
        { readcallback_ = cb; }
        void  setsReadCallback(const EventCallBack &cb)
        { sreadcallback_ = cb; }
        void setWriteCallback(const EventCallBack &cb)
        { writecallback_ = cb; }
        int getSockfd() { return sockfd_; }
        void setSockfd(int sockfd) { sockfd_ = sockfd; }
        void setRevents(int revt) { revents_ = revt; }
        void setIndex(int index) { index_ = index; }
        int getEvents() { return events_; }
        int getIndex() { return index_; }
        bool isNoneEvent() { return events_ == NoneEvent_; }

        void enableReading() { events_ |= ReadEvent_; update();}
        void enableWriting() { events_ |= WriteEvent_; update(); }
        bool isWriting() { return events_& WriteEvent_; }
        void disableWriting() { events_ &= ~WriteEvent_; update(); }
        void disableAll() { events_ = NoneEvent_; update(); }
    private:
        EventLoop *loop_;
        void update();
        static const int NoneEvent_;
        static const int ReadEvent_;
        static const int WriteEvent_;
        int sockfd_;
        int revents_;
        int events_;
        int index_;
        EventCallBack readcallback_;
        EventCallBack sreadcallback_;
        EventCallBack writecallback_;
};
#endif