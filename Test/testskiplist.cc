#include "../Server/util/SkipList.h"

int main() {
    srand(time(NULL));
    skiplist s(1,0.5);
    string s1 = "hee";
    string s2 = "dddd";
    string s3 = "ddddddd";
    s.insertNode(s1.size(),s1);
    s.insertNode(s2.size(),s2);
    s.insertNode(s3.size(),s3);
    // s.deleteNode(s1.size(),s1);
    cout << s.getNodeValue(s2.size()) << endl;
    cout << "Rank" << s.getNodeRank(s3.size(),s3);
    cout << "Length: " << s.getLength() << endl;
    rangeSpec c(3,5);
    cout << "RangeLength: " << s.getCountRange(c) << endl;
    auto v = s.getNodeRange(c);
    for(auto& i : v) {
        cout << "score: " << i->score_ << " " << "value: " << i->key_ << endl;
    }
    // if(s.isInRange(c)) {
    //     cout << "get range" << endl;
    // } else {
    //     cout << "out range" << endl;
    // }
    // s.deleteNodeRange(c);
    s.displayNode();

}