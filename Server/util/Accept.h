#ifndef _ACCEPT_H_
#define _ACCEPT_H_
#include "modelHead.h"
#include "SocketOpts.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Buffer.h"
#include "Channel.h"


class InetAddr;
// class Channel;
class EventLoop;

class Accept : public std::enable_shared_from_this<Accept>{
    public:
        typedef std::function<void(int sockfd,
                                       const Address &)> NewConnectionCallBack;
        typedef std::function<void(const AcceptorPtr&)> ConnecttionCallBack;
        typedef std::function<void(const AcceptorPtr &conn,
                                        Buffer *buf,ssize_t len)> MessageCallBack;
       
        typedef std::function<void()> CloseCallBack;

        Accept(EventLoop* loop,const Address&);
        
        void NewConnection(int sockfd,const Address& peeraddr) {
            std::cout << peeraddr.toIp() << std::endl;
            addr_ = peeraddr;
            shared_from_this()->setCloseCallBack(std::bind(&Accept::handleClose,this));
            std::cout << "GetSockfd: " << sockfd << std::endl;
            setChannelAccepted(std::shared_ptr<Channel>(new Channel(loop_,sockfd)));
            
        }
        std::string getSocketInfo() {
            return addr_.toIpPort();
        }
        void setConnectionCallBack(const ConnecttionCallBack &cb) 
        { connectioncallback_ = cb; }
        void setMessageCallBack(const MessageCallBack &cb)
        { messagecallback_ = cb; }
        void setCloseCallBack(const CloseCallBack &cb)
        { closecallback_ = cb; } 
        void setChannelAccepted(const std::shared_ptr<Channel> &channelAcccepted) {  
            channelAccepted_ = channelAcccepted;
            setState(Connected);
            channelAccepted_->setsReadCallback(std::bind(&Accept::handleRead,this));
            channelAccepted_->setWriteCallback(std::bind(&Accept::handleWrite,this));
            channelAccepted_->enableReading();
            
            // AcceptorPtr conn(new Accept(loop_,addr_));
            // conn->channelAccepted_ = channelAcccepted;
            // conn->channelAccepted_->setsReadCallback(std::bind(&Accept::handleRead,this));
            // conn->setConnectionCallBack(connectioncallback_);
            // conn->setMessageCallBack(messagecallback_);
            // conn->setCloseCallBack(std::bind(&Accept::handleClose,this)); 
            // conn->channelAccepted_->enableReading();
        }
        void removeChannel() { std::bind(&Accept::handleClose,this); }
        bool listening() { return listening_; }
        void listen();
        void send(const std::string &message);
    private:
        enum State {Connecting, Connected, disConnected, disConnecting};
        void setNewConnectionCallBack(const NewConnectionCallBack &cb)
        { newconnectioncallback_ = cb;}
        void handleEstablishRead();
        void handleRead();
        void sendInLoop(const std::string &messages);
        void shutdown();
        void shutdownInLoop();
        void handleWrite();
        void handleClose();
        State state_;
        void setState(State state) { state_ = state; }
        EventLoop* loop_;
        Socket acceptSocket_;
        Channel acceptChannel_;
        std::shared_ptr<Channel> channelAccepted_;
        NewConnectionCallBack newconnectioncallback_;
        ConnecttionCallBack connectioncallback_;
        MessageCallBack messagecallback_;
        CloseCallBack closecallback_;
        bool listening_;
        Address addr_;
        Buffer input_;
        Buffer output_;
};
#endif