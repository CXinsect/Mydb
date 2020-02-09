#include "../Server/model.h"

int main (void) {
    char buf[1024] = {0};
    std::string ptr = getcwd(buf,sizeof(buf));
    std::cout << "ptr: " << ptr  <<"\nbuf: " << buf << std::endl;
}