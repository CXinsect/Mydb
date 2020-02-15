#include "../Client/Client.h"
#include <ctime>
int main (int argc,char* argv[]) {
    Client client(6379,"127.0.0.1",600);
    client.Connect();
    clock_t startTime = clock();
    int i;
    while(1) {
        for(i = 0;i < atoi(argv[1]);i++) {
            client.sendRequest("set name test");
        }
        if(i == atoi(argv[1])) break;
    }
    clock_t endTime = clock();
    std::cout << "Time costs: " << (double) (endTime - startTime)/CLOCKS_PER_SEC << std::endl;
}