#include <iostream>

int main (void) {
    unsigned char buf[2];
    int len = 6;
    buf[0] = len &0xFF;
    int t = buf[0];
    std::cout << t << std::endl;
    return 0;
}