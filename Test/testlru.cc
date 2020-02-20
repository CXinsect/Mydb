#include "../Server/util/LRU.h"
#include <list>
int main (void) {
    LRUCache<int,int> l(5);
    l.set(1,1);
    l.set(2,2);
    l.set(3,3);
    l.set(4,4);
    l.set(5,5);
    l.set(6,6);
    if(l.get(2) == -1) cout << "not found" << endl;

    cout << l.getCount() << endl;
}