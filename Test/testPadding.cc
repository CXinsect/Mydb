#include <iostream>

int  cal (int size) {
   int model = 8;
    return (size+8-1) &~(8-1);
}
int main (void) {
    
    std::cout << "value: " << cal(123) << std::endl;
    return 0;
}
