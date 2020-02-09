#ifndef __THREAD_POOL_
#define __THREAD_POOL_
#include <iostream>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <memory>
using namespace std;

// template <typename T>
struct Task {
    public:
        Task() {}
        ~Task() {}
        virtual void Run ()=0;
};
class ThreadPool {
    public:
        ThreadPool (int nums) : numThreads_ (nums), stoped_(false) {}
        ~ThreadPool() {}
        void addTask (Task* task) {
            
            unique_lock<mutex> mylock (mutex_);
            queue_.push(task);
            cond_.notify_one();
        } 
        void processTask () {
            while(!stoped_) {
                unique_lock<mutex> mylock (mutex_);
                cond_.wait(mylock,[this] { if(queue_.empty() && !isStoped()) return false; else return true; });
                // cond_.wait(mylock);
                if(queue_.empty()) continue;
                Task* task = queue_.front();
                queue_.pop();
                if(task == nullptr) continue;
                mylock.unlock();
                this_thread::sleep_for(chrono::milliseconds(500));
                task->Run();

                mutex_.lock();
                if(!isStoped()) delete task;
                mutex_.unlock();
                
            }
            cout << "hello linux" << endl;
        }
        void Start() {
            cout << "开始创建线程 " << endl;
            for(int i = 0;i < numThreads_;i++) {
                thread_.push_back(thread(&ThreadPool::processTask,this));
            }
        }

        void Stop() {
            stoped_ = true;
            cond_.notify_all();
            for(auto &i:thread_) i.join();
            while(!queue_.empty()) {
                this_thread::sleep_for(chrono::milliseconds(500));
            }
            unique_lock<mutex> mylock(mutex_);
            while(!queue_.empty()) {
                Task* task = queue_.front();
                queue_.pop();
                delete task;
            }
        }
        bool isStoped () { return stoped_; }
    private:
        int numThreads_;
        bool stoped_;
        queue<Task*> queue_;
        mutex mutex_;
        condition_variable cond_;
        vector <thread> thread_;
};

#endif