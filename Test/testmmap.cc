#include <sys/mman.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
using namespace std;

int main (int argc,char* argv[]) {
    struct stat st;
    assert(stat(argv[1],&st) != -1);
    int fd = open(argv[1],O_RDONLY,0644);
    assert(fd != -1);
    char* m = static_cast<char*>(mmap(0,st.st_size,PROT_READ,MAP_SHARED,fd,0));
    string str(m,st.st_size);
    close(fd);
    cout << "str: " << str << m << endl;
    return 0;
}