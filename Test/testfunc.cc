#include <iostream>
#include <functional>
using namespace::std::placeholders;

void test(int) {
    std::cout << "test" << std::endl;
}
void test1(int,int) {
    std::cout << "test1" << std::endl;
}
class Test {
    public:
        typedef std::function<void(int,int)> f;
        void test(int) {
            std::cout << "test" << std::endl;
        }
        void test1(int,int) {
            std::cout << "test1" << std::endl;
        }
        void setCallback(const f&f_) {
            callback_ = f_;
        }
        void print(int a,int b) {
            if(a == b)
                setCallback(std::bind(&Test::test,this,a)); 
            else 
                setCallback(std::bind(&Test::test1,this,a,b)); 
            callback_(a,b);               
        }
        f callback_;
        
};
std::function<void(int,int)> f;
int main () {
    Test a;
    a.print(1,1);
    a.print(1,2);
    return 0;
}