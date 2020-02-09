#include <memory>
#include <iostream>

using namespace std;

class A {
    public:
        A(shared_ptr<int> a) : a_(a) {}
        void print() {
            if(a_.lock())
            cout << *a_.lock() <<endl;
        } 
    private:
        weak_ptr<int> a_;
};
int main (void) {
    shared_ptr<int> s1 (new int(1));
    weak_ptr<int> w(s1);
    weak_ptr<int> w1(shared_ptr<int>(new int(2)));
   
    shared_ptr<int> s(new int(2));
    if(w.lock()) {
        // w.reset();
        w.swap(w1);
        cout << w.use_count() << endl;
    } else {
        cout << *w.lock() << endl;
    }
    A a(s1);
    cout << "linux" << endl;
    a.print();
    return 0;
}