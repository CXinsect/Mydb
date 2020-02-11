#include "../Server/util/skiplist.h"

int main() {
    srand(time(NULL));
    skiplist s(3,0.5);
    string s1 = "hee";
    string s2 = "dddd";
    string s3 = "ddddddd";
    s.insertNode(s1.size(),s1);
    s.insertNode(s2.size(),s2);
    s.insertNode(s3.size(),s3);
    s.deleteNode(s1.size());
    cout << s.getNodeValue(s2.size()) << endl;
    s.displayNode();

}