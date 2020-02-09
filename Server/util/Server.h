#ifndef _SERVER_H_
#define _SERVER_H_

#include "Accept.h"
#include "EventLoop.h"
#include "status.h"

#include <unordered_map>

using namespace Mydb;
using namespace std;

const int OptionSelectdb = 254;
const int OptionEof = 255;
const int OptionExpireTime = 252;

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
    private:
        const std::string getCommand(const vector<string>&);
        const std::string setCommand(const vector<string>&);
        const std::string bgsaveCommand(const vector<string>&);
        const std::string delCommand(const vector<string>&);
        const std::string selectCommand(const vector<string>&);
        const std::string expireTimeCommand(const vector<string>&);
        const std::string rpushCommand(const vector<string>&);
        const std::string rpopCommand(const vector<string>&);
        const std::string hsetCommand(const vector<string>&);
        const std::string hgetCommand(const vector<string>&);
        const std::string hgetallCommand(const vector<string>&);
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
         void saveHead(ofstream& out) {
             string tmp = "REDIS0004";
             out.write(tmp.c_str(),tmp.size());
         }
         void saveType(ofstream& out,unsigned char data) {
             out.write(static_cast<char*>(static_cast<void*>(&data)),1);
         }
         void saveLength(ofstream& out,int len) {
            unsigned char buf[2];
            if (len < (1<<6)) {
                //保存6位数据
                buf[0] = len & 0xFF;
                out.write(static_cast<char*>(static_cast<void*>(buf)),1);
            } else {
                cout << "The Length is Too Long [abort]" << endl;
                abort();
            }
         }
         void saveExpireTime(ofstream& out,long long Time) {
             char buf[16];
             sprintf(buf,"%lld",Time/1000);
             saveType(out,OptionExpireTime);
             saveLength(out,strlen(buf));
             out.write(buf,strlen(buf));
         }
         void saveSelectDb(ofstream& out,int index) {
             char buf[16];
             sprintf(buf,"%d",index);
             saveType(out,OptionSelectdb);
             saveLength(out,strlen(buf));
             out.write(buf,strlen(buf));
         }
         void saveKey(ofstream& out,unsigned char type,const string key) {
            saveType(out,type);
            saveLength(out,key.size());
            out.write(key.c_str(),key.size());
         }
         void saveValue(ofstream& out,unsigned char encoding,const string value) {
            saveType(out,encoding);            
            saveLength(out,value.size());
            out.write(value.c_str(),value.size());
         }
         void saveTail(ofstream& out) {
             saveType(out,OptionEof);
         }

    public:
        //Rdb的数据结构
        const std::string Redis = "REDIS";
        const std::string Version = "0004";
        long long lastsave_ = 0;
        bool sigStop_ = false;

    private:
        EventLoop *loop_;
        std::shared_ptr<Accept> accept_;
        unordered_map <string,function<string(const vector<string>&)>> cmdtable_;
        
        int dbIndex_  = -1;
        int dbSize_;
        int maxIndex_ = 16;
        std::vector <shared_ptr<DataBase>> database_;
        //temporary structure
        int keylen_;
        int skeylen_;
        int valuelen_;
        std::string cmd_;
        std::string key_;
        std::string skey_;
        std::string value_;
public:
        int cmdLen_;
private:
    Status status_;
};


#endif