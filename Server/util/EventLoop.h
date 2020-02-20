#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_

#include "Epoll.h"
#include "ModelHead.h"

class EventLoop {
 public:
  EventLoop() : looping_(false), epoll_(new Epoll) {
    int epd = ::epoll_create(10);
    epoll_->setEpollfd(epd);
  };
  ~EventLoop() { assert(!looping_); };
  typedef std::vector<class Channel*> ChannelList;
  bool quit_;
  std::shared_ptr<Epoll> epoll_;
  void loop();
  void updateChannel(Channel* channel);
  void quit() { quit_ = true; }
  void runInLoop(const std::function<void()>& cb) { cb(); }
  void removeChannel(Channel* channel) { epoll_->removeChannel(channel); }

 private:
  bool looping_;
  ChannelList activeChannels_;
};
#endif