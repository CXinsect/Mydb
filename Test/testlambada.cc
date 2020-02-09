#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

string intToString (int t) {
            string s = "";
            while(t) {
                s += '0' + t % 10;
                t /= 10;
            }
            reverse(s.begin(),s.end());
            return s;
}
int main (void) {
    int a = 1;
    std::string tmp = "dd";
    std::string str = "hello,world";
    str += intToString(a);
    str += tmp;
    std::cout << str<< std::endl;
    return 0;
}