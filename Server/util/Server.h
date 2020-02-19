#ifndef _SERVER_H_
#define _SERVER_H_

#include "Accept.h"
#include "EventLoop.h"
#include "status.h"
#include <unordered_map>

using namespace Mydb;
using namespace std;

const string OptionSelectdb = "FE";
const string OptionExpireTime = "FD";
const string OptionEof = "EOF";

class DataBase;
class Server {
    public:
        Server(EventLoop *loop,const Address &peeraddr) : 
                    loop_(loop),
                    accept_(std::shared_ptr<Accept>(new Accept(loop,peeraddr)))
        { 
            accept_->setConnectionCallBack(std::bind(&Server::onConnection,this,_1));
            accept_->setMessageCallBack(std::bind(&Server::onMessage,this,_1,_2,_3));
            accept_->listen();
            Init();
        }
        ~Server() {
            while(waitpid(-1,NULL,WNOHANG) != -1);
        }
    private:
        const string getCommand(const vector<string>&);
        const string setCommand(const vector<string>&);
        const string bgsaveCommand(const vector<string>&);
        const string delCommand(const vector<string>&);
        const string selectCommand(const vector<string>&);
        const string expireTimeCommand(const vector<string>&);
        const string rpushCommand(const vector<string>&);
        const string rpopCommand(const vector<string>&);
        const string hsetCommand(const vector<string>&);
        const string hgetCommand(const vector<string>&);
        const string hgetallCommand(const vector<string>&);
        const string zaddCommand(const vector<string>&);
        // const string zcardCommand(const vector<string>&);
        // const string zcountCommand(const vector<string>&);
        // const string zrangeCommand(const vector<string>&);
        static void endDataBase(DataBase *) { ; }
    public:
        void Init() ;
        void onConnection(const AcceptorPtr& conn);
        void onMessage(const AcceptorPtr& conn,Buffer *buf,ssize_t n);
        std::string commandRequest(Buffer *buf);
        void commandReply(Buffer *buf);
        vector<shared_ptr<DataBase>> getDataBase() { return database_; }
public:
    static void parentHandle(int sig);
    void rdbSave();
    bool CheckStorageConditions();
    private:
         long long  getTimestamp() {
             struct timeval tv;
             assert(gettimeofday(&tv, NULL) != -1);
             return tv.tv_sec;
         }
         string saveHead() {
             string tmp = "REDIS0004";
             return tmp;
         }
         string saveSelectDb(int index) {
             return OptionSelectdb + to_string(index);
         }
         string saveExpireTime(long long Time) {
             return OptionExpireTime + to_string(Time);
         }
         string saveType(short type) {
             return string ("^" + to_string(type));
         }
         string saveKeyValue(const string& key,const string& value) {
            char buf[1024];
            sprintf(buf,"!%d#%s!%d$%s",(int)key.size(),key.c_str(),
                                                    (int)value.size(),value.c_str());
            return string(buf);
         }

    public:
        //Rdb的数据结构
        const std::string Redis = "REDIS";
        const std::string Version = "0004";
        long long lastsave_ = 0;
        bool sigStop_ = false;

    private:
        EventLoop *loop_;
        shared_ptr<Accept> accept_;
        unordered_map <string,function<string(const vector<string>&)>> cmdtable_;
        vector <shared_ptr<DataBase>> database_;        
        int dbIndex_  = -1;
        int dbSize_;
        int maxIndex_ = 16;
        //temporary structure
        int keylen_;
        int skeylen_;
        int valuelen_;
        string cmd_;
        string key_;
        string skey_;
        string value_;
public:
        int cmdLen_;
private:
    Status status_;
};


#endif