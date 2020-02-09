#include "../Server/util/modelHead.h"
#include "../Client/Client.cc"
#include <sstream>

int main (void) {
    char buf[1024];
    // while(1) {
    //     fgets(buf,sizeof(buf),stdin);
    //     std::istringstream str(buf);
    //     std::string a,b;
    //     str >> a;
    //     str >> b;
    //     std::cout << buf << std::endl;
    // }
    Client client(6379,"127.0.0.1",600);
    int ret = client.Connect();
    assert(ret != -1);
    client.getInput();
    return 0;
}