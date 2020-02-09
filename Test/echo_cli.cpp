/*************************************************************************
	> File Name: echo_cli.cpp
	> Author: 
	> Mail: 
	> Created Time: 2019年06月16日 星期日 11时57分22秒
 ************************************************************************/
#include "model.h"
using namespace std;
int  connect(int port) {
    struct sockaddr_in  serv;
    memset(&serv,0,sizeof(serv));
    int sockfd;
    if((sockfd = socket(PF_INET,SOCK_STREAM,0)) < 0) 
        perror("socket");
    serv.sin_family = PF_INET;
    serv.sin_port = htons(port);
    serv.sin_addr.s_addr = 0;
    if(connect(sockfd,(struct sockaddr *)&serv,sizeof(serv)))
        perror("connect");
    return sockfd;
}
int main(int argc,char * argv[])
{
    if(argc < 2) {
        cout<<"The index is wrong"<<endl;
        exit(0);
    }
    int sockfd = connect(atoi(argv[1]));
    char buf[1024] = {0};
    char recvbuf[1024] = {0};
    char sendbuf[1024] = "hello,Linux";
    int nwrite;
   /* nwrite = write(sockfd,sendbuf,strlen(buf));
    std::cout << nwrite << errno << std::endl;
    while((nwrite = write(sockfd,sendbuf,strlen(buf))) > 0);*/

    int n;
    n = write(sockfd,"Linux",10);
    std::cout << recvbuf << "n: "<< n <<  std::endl;
    int count = 0;
    while((n = read(sockfd,recvbuf,sizeof(recvbuf)))) {
        std::cout << "recvbuf" << recvbuf << n <<std::endl;
        memset(recvbuf,0,sizeof(recvbuf));
        if(n == 0) {
            std::cout << "Server is Over" << std::endl;
            break;
        }
    }
    close(sockfd);
    return 0;
}
