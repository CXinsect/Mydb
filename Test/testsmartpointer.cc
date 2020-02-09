#include "../Server/model.h"

void endInt(int *) {
    std::cout << "hello" << std::endl;
    ;
}
int main (void) {
    // std::map <std::pair<int,int>,int> m;
    // m.insert(std::make_pair(std::make_pair(1,1),1));
    // std::map <std::pair<int,int>,int>::iterator it = m.begin();
    // (*it).first.first = 1;
    std::string tmp = "rpush name lala";
    std::istringstream str(tmp);
    std::string key,value;
    str >> key;
    str >> value;
    std::cout << value << std::endl;
    int pos = tmp.find(value);
    std::cout << pos << std::endl;
    return 0;
}