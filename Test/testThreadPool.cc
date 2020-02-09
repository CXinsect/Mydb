#include "threadpool.h"

struct A : public Task {
    public:
        A(int d) : a(0) {}
        ~A() {}
        void Run() override {
            print();
        }
        void print();
    private:
        int a;
};
void A::print() {
    cout << a << endl;
}
int main (void) {
    ThreadPool* t = new ThreadPool(8);
    t->Start();
    for(int i = 0;i <8;i++)
        t->addTask(new A(1));
    
  
    t->Stop();
    delete t;
    // A a;
}