#include "Accept.h"
#include "EventLoop.h"
Accept::Accept(EventLoop* loop, const Address& listenAddr):
                                    loop_(loop),
                                    acceptSocket_(sockets::creatNonblocking(AF_INET)),
                                    acceptChannel_(loop,acceptSocket_.getSockfd()),
                                    listening_(false)
{
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.bindAddress(listenAddr);
    setNewConnectionCallBack(std::bind(&Accept::NewConnection,this,_1,_2));
    acceptChannel_.setReadCallback(std::bind(&Accept::handleEstablishRead,this));
}
void Accept::listen() {
    listening_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading();
}
void Accept::handleEstablishRead () {
    Address peerAddr(0,false);
    int confd = acceptSocket_.accept(&peerAddr);
    assert(confd >= 0);
    std::cout << "connfd" << confd << std::endl;
    if(confd >= 0) {
        if(newconnectioncallback_)
            newconnectioncallback_(confd,peerAddr);
        else {
            sockets::close(confd);
        }
    }
}
void Accept::handleRead () {
    ssize_t n = input_.readFd(channelAccepted_->getSockfd());
    std::cout << "length: " << n << std::endl;
    if(n > 0) {
        messagecallback_(shared_from_this(),&input_,n);
    } else if(n == 0)
        handleClose();
    else {
        std::cout << "[ACCEPT::handleRead] error: " << __LINE__ << std::endl;
        return;
    }
}
void Accept::shutdown() {
    if(state_ == disConnected) {
        setState(disConnecting);
        loop_->runInLoop(std::bind(&Accept::shutdownInLoop,this));
    }
}
void Accept::shutdownInLoop () {
    if(!channelAccepted_->isWriting()) {
        acceptSocket_.shutdownWrite();
    }
}
void Accept::send(const std::string &message) {
    // assert(state_ == Connected);
    if(state_ == Connected) {
        sendInLoop(message);
    }
}
void Accept::sendInLoop (const std::string &message) {
    ssize_t nwrite = 0;
    if(!channelAccepted_->isWriting() && output_.getReadableBytes() == 0) {
        nwrite = ::write(channelAccepted_->getSockfd(),message.data(),message.size());
        if(nwrite >= 0) {
            if((int)nwrite >= 0) {
                std::cout << "The data has not been sent, waiting for the next time to continue" << std::endl;
            }
        } else {
            nwrite = 0;
            if(errno == EWOULDBLOCK) {
                std::cout << "Beyond reason, unexpected" << std::endl;
            }
        }
    }
    assert(nwrite >= 0);
    if((size_t)nwrite < message.size()) {
        output_.Append(message.data()+nwrite,message.size()-nwrite);
        if(!channelAccepted_->isWriting()) {
            channelAccepted_->enableWriting();
        }
    }
}
void Accept::handleWrite () {
    if(!channelAccepted_->isWriting()) {
        ssize_t n = ::write(channelAccepted_->getSockfd(),
                                output_.peek(),
                                output_.getReadableBytes());
        if(n > 0) {
            output_.retrieve(n);
            if(output_.getReadableBytes() == 0) {
                channelAccepted_->disableWriting();
                if(state_ == disConnecting) {
                    shutdownInLoop();
                }
            } else {
                std::cout << "More Date will be sent" << std::endl;
            }
        } else {
            std::cout << "[Accept::handwrite]: " << __LINE__ << std::endl;
        }
    } else {
        std::cout << "[Accept::handwrite]: Connection has been closed " << __LINE__ << std::endl; 
    }
}
void Accept::handleClose() {
    assert(state_ == Connected);
    setState(disConnected);
    channelAccepted_->disableAll();
    connectioncallback_(shared_from_this());
    std::cout << "Get(): " << channelAccepted_.get()->getSockfd() << " : " << channelAccepted_->getSockfd() << std::endl;
    loop_->removeChannel(channelAccepted_.get());
    std::cout <<"Channel has been Closed" << std::endl;
}