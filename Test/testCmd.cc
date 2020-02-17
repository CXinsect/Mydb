#include "../Client/Client.h"
#include <ctime>
int main (int argc,char* argv[]) {
    Client client(6379,"127.0.0.1",600);
    client.Connect();
//set
    clock_t startTime = clock();
    int i;
    while(1) {
        for(i = 0;i < atoi(argv[1]);i++) {
            char buf[11];
            snprintf(buf,10,"t%d",i);
            string t = "set name ";
            t += string(buf);
            client.sendRequest(t);
           
        }
        if(i == atoi(argv[1])) break;
    }
    clock_t endTime = clock();

//get    
    clock_t startTime1 = clock();
    while(1) {
        for(i = 0;i < atoi(argv[1]);i++) {
            client.sendRequest("get name");
           
        }
        if(i == atoi(argv[1])) break;
    }
    clock_t endTime1 = clock();
    std::cout << "set time costs: " << (double) (endTime - startTime)/CLOCKS_PER_SEC << std::endl;
    std::cout << "get time costs: " << (double) (endTime1- startTime1)/CLOCKS_PER_SEC << std::endl;
}