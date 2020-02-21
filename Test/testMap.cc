#include <iostream>
#include <map>
#include <list>
#include <unordered_map>
#include <ext/pool_allocator.h>

using namespace std;
template <typename T> 
using listPool = list<T,__gnu_cxx::__pool_alloc<T>>;
template<typename key,typename T>
using MapPool = unordered_multimap<key,T,std::hash<key>,std::equal_to<T>,__gnu_cxx::__pool_alloc<pair<const key,T>>>;
template <typename key,typename T>
using mapPool = unordered_map<key,T,std::hash<key>,std::equal_to<key>,__gnu_cxx::__pool_alloc<pair<const key,T>>>;
int main (void) {
    mapPool<int,int> v;
    v.insert(make_pair(1,2));
    auto it = v.begin();
    cout << it->first << it->second << endl;
    mapPool<int,MapPool<int,int>> v1;
    MapPool<int,int> t;
    multimap<int,int> d;
    t.insert(make_pair(1,2));
    v1.insert(make_pair(1,t));
    auto iter = v1.begin();
    auto tmp = iter->second;
    auto tmp1 = tmp.find(1);
    cout << "map: " << iter->first << tmp1->first << tmp1->second << endl;
    auto beg = t.equal_range(1);
    while(beg.first != beg.second) {
        cout << "unordered_: " << beg.first->first << beg.first->second << endl;
        beg.first++;
    }
    return 0;
}