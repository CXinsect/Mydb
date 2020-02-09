#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <unistd.h>
#include <features.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>
# define PRIx64 __PRI64_PREFIX "x"
using namespace std;
int main (void) {
    char buf[64];
    sprintf(buf,"%08" PRIx64,1);
    cout << buf << endl;
    int i = 1;
    assert(i == 1);
    cout << "hello,world" << endl;

    unique_ptr<int> a(new int(5));
    unique_ptr<int> b(new int(5));

    // unique_ptr<int> b = std::move(a);

    // unique_ptr<int> b(std::move(a));
    int c = 5;
    int* d = &c;
    b.reset(new int(6));
    cout << "a：　"<<  "b: " << *b << endl; 

    return 0;
}
