//
// Created by insect on 2019/12/13.
//


#include <iostream>
#include <map>

using namespace std;

int main (void) {
    map<int,pair<int,int>> test;
    multimap<int,int> t;
    test.insert(make_pair(3,make_pair(1,2)));
//    test.insert(3,make_pair(3,t));
//    auto testIter = test.find(3);
//    testIter->second.insert(make_pair(1,3));
//    map<int,multimap<int,int>>::iterator it   = test.find(3);
//
//
//
//    while(it != test.end()) {
//        multimap<int,int>::iterator iter = it->second.find(1);
//        int count = it->second.count(1);
//        while(count) {
//            cout << iter->first << iter->second << endl;
//            iter++;
//            count--;
//
//        }
//        it++;
//    }
//    for(auto pos = testIter->second.equal_range(1);pos.first != pos.second;pos.first++) {
//        cout << pos.first->first << pos.first->second << endl;
//    }
//
//    for(auto begin = testIter->second.lower_bound(1),end = testIter->second.upper_bound(1);begin != end; begin++ )
//        cout << begin->first << begin->second << endl;

//    int count = *(it->second).count(1);
//    while(count) {
//        auto iter = *it.equal_range(1);
//        while(iter.first != iter.second) {
//            cout << iter.first->second << " ";
//            iter.first++;
//        }
//        count--;
//        cout << endl;
//    }

}


