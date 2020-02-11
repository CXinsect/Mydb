#ifndef _EPOLL_H_
#define _EPOLL_H_
#include "modelHead.h"
#include "Channel.h"
class Epoll {
    public:
        typedef std::vector<class Channel*> ChannelList;
        Epoll()=default;
        ~Epoll() {}
        void setEpollfd(int epollfd) { epollfd_ = epollfd; }
        void epoll(ChannelList *activeChannel);
        void fillActiveChannel(int numEvents, ChannelList* activeChanel);
        void updateChannel(Channel *channel);
        void removeChannel(Channel *channel);

    private:
        typedef std::vector<struct epoll_event> EventList;
        typedef std::map<int, class Channel*>ChannelMap;
        int epollfd_;
        EventList events_;
        ChannelMap channel_;
};
#endif