#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <functional>
#include <memory>
std::vector <int> ::iterator func(std::vector <int> & v) {
    // std::vector <int>  v= {1,2,3};
    std::vector<int>::iterator it = v.end();
    return it;
}
int main () {
    // std::vector <int> v = {1,2,3};
    // std::vector <int> ::iterator it = func(v);
    
    // if(it == v.end())
    //     std::cout << " champaign " << std::endl;
    std::list<std::string> tmp;
    tmp.push_back("cx");
    tmp.push_back("lala");
    tmp.push_back("nini");
    std::map<std::pair<std::string, int>,std::pair<std::list<std::string>,int>> v;
    v.insert(make_pair(std::make_pair("name",1),make_pair(tmp,0))); 
    v.insert(make_pair(std::make_pair("xinmg",1),make_pair(tmp,0))); 
    v.insert(make_pair(std::make_pair("age",1),make_pair(tmp,0)));
    int size = -1;
    std::map<std::pair<std::string, int>,std::pair<std::list<std::string>,int>>::iterator it = v.begin();
    while(it != v.end()) {
        if(it->first.first == "xinmg")
            break; else {
                size++;
                it++;
            }
    }
    std::cout << size << std::endl;
    if(it == v.end())
        std::cout << "Not Found" << std::endl;
    std::list<std::string> ::iterator iter = it->second.first.begin();
    while(size-- > 0) iter++;
    std::cout << *iter << size << std::endl;
    return 0;
}