#ifndef _CLIENT_H_
#define _CLIENT_H_
#include "../Server/util/modelHead.h"
class Io
{
public:
  static size_t writen(int sockfd, char *buf, ssize_t count);
  static size_t readn(int sockfd, char *buf, ssize_t count);
};
const int TimeValue = 60;
class Client {
    public:
        Client(int port,const std::string &ip,int _time):
                                port_(port),
                                Ip_(ip),
                                time_(_time)
                                {}
        ~Client() { close(confd_); }
        int Connect();
        
        void getInput() {
            char buf[1024] = {0};
            std::cout << "Input quit to Stop" << std::endl;
            int position = 0;
            if(position == 0)
              std::cout << "Redis >" << " ";
            else
              printf("Redis[%d] >",position);
            while(fgets(buf,sizeof(buf),stdin) != NULL) {
                std::string temp(buf,buf+strlen(buf));
                int pos = temp.find_last_of('\n');
                temp = temp.substr(0,pos);
                if(!strcmp(temp.c_str(),"quit"))
                    break;
                std::cout << "test" << std::endl;
                const std::string tmp = buf;
                sendRequest(tmp);
                std::cout << "Input quit to Stop" << std::endl;
                std::istringstream str(tmp);
                std::string tselect;
                str >> tselect;
                if(tselect == "select") {
                  pos = tmp.find_first_of(' ');
                  position = atoi(tmp.substr(pos+1,tmp.size()).c_str());
                }
                 if(position == 0)
                  std::cout << "Redis >" << " ";
                else
                  printf("Redis[%d] >",position); 
            }
        }
        void sendRequest(const std::string &buf);
    private:
        int setNoOrBlocking(int fd);
        void AuxiliaryFun(char *buffer) {
            int ret = Io::writen(confd_,buffer,strlen(buffer));
            assert(ret == strlen(buffer));
            memset(buffer,0,1024);
            // setNoOrBlocking(confd_);
            // ret = Io::readn(confd_,buffer,1024);
            ::read(confd_,buffer,1024);
            assert(ret != -1);
            std::cout << "Response: " << buffer+1;
        }
    private:
        int port_;
        const std::string Ip_;
        int confd_;
        int time_ = TimeValue;
};

#endif