#ifndef __SKIPLIST_H_
#define __SKIPLIST_H_
#include <iostream>
#include <string>
#include <cstring>
#include <memory>
using namespace std;
class skiplistNode;
class skiplistLevel {
    public:
        skiplistLevel() : forward_(nullptr),span_(0) {}
    public:
        skiplistNode* forward_;
        unsigned int span_;
};

class skiplistNode {
    public:
        skiplistNode(double score,const string& key,int level) {
            this->score_ = score;
            this->key_ = key;
            level_ = unique_ptr<skiplistLevel*[]>(new skiplistLevel*[level+1]);
            for(int i = 0;i < level;i++) {
                level_[i] = new skiplistLevel;
                memset(level_[i],0,sizeof(skiplistLevel));
            }
        }
    public:
        unique_ptr<skiplistLevel*[]> level_;
        double score_;
        string key_;
};

class skiplist {
    public:
        skiplist(int max,float p) : maxlvl_(max),p_(p),lvl_(1),head_(new skiplistNode(-1,"",max)){}
        ~skiplist() {

        }
        int randomLevel();
        skiplistNode* creatNode(double score,const string& key,int level);
        void insertNode(double score,const string& key);
        void deleteNode(double score);
        const string getNodeValue(double score);
        void displayNode();
    private:
        int maxlvl_;
        float p_;
        int lvl_;
        skiplistNode* head_,*tail_;
};
#endif