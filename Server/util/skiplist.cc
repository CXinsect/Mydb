#include "skiplist.h"

int skiplist::randomLevel() {
    float r = (float)rand() /RAND_MAX;
    int level = 0;
    while(r < p_ && level < maxlvl_) {
        level++;
        r = (float)rand() / RAND_MAX;
    }
    return level;
}

skiplistNode* skiplist::creatNode(double score,const string& key,int level) {
    skiplistNode* node = new skiplistNode(score,key,level);
    return node;
}

void skiplist::insertNode(double score,const string& key) {
    skiplistNode* cur = head_;
    skiplistNode* update[maxlvl_+1];
    memset(update,0,sizeof(skiplistNode*)*(maxlvl_+1));
    //从顶层链表向下层遍历，查找插入位置
    for(int i = lvl_-1;i >= 0;i--) {
        while(cur->level_[i]->forward_ != NULL && cur->level_[i]->forward_->score_ < score) {
            cur = cur->level_[i]->forward_;
        }
        update[i] = cur;
    }
    cur = cur->level_[0]->forward_;
    if(cur == NULL || cur->score_ != score) {
        int rlevel = randomLevel();
        //如果新节点层数高于现有节点则更新指针信息
        if(rlevel > lvl_) {
            for(int i = lvl_;i < rlevel;i++) {
                update[i] = head_;
            }
            lvl_ = rlevel;
        }
        //链表插入过程
        skiplistNode* node = creatNode(score,key,lvl_);
        for(int i = 0;i < lvl_;i++) {
            node->level_[i]->forward_ = update[i]->level_[i]->forward_;
            update[i]->level_[i]->forward_ = node;
        }
    }
}

void skiplist::deleteNode(double score) {
    skiplistNode* cur = head_;
    skiplistNode* update[maxlvl_+1];
    memset(update,0,sizeof(skiplistNode*)*(maxlvl_+1));
    //遍历过程
    for(int i = lvl_-1;i >= 0;i--) {
        while(cur->level_[i]->forward_ != NULL && cur->level_[i]->forward_->score_ < score) {
            cur = cur->level_[i]->forward_;
        }
        update[i] = cur;
    }
    cur = cur->level_[0]->forward_;
    if(cur && cur->score_ == score) {
        for(int i = 0;i < lvl_;i++) {
            if(update[i]->level_[i]->forward_ != cur) 
                break;
            update[i]->level_[i]->forward_ = cur->level_[i]->forward_;
        }
    }
    delete cur;

    while(lvl_ > 1 && head_->level_[--lvl_]->forward_ == NULL) {
        lvl_--;
    }
}
const string skiplist::getNodeValue(double score) {
    skiplistNode* cur = head_;
    for(int i = lvl_-1;i >= 0;i--) {
        while(cur->level_[i]->forward_ && cur->level_[i]->forward_->score_ < score) {
            cur = cur->level_[i]->forward_;
        }
    }
    cur = cur->level_[0]->forward_;
    if(cur->score_ == score) {
        return cur->key_;
    } else {
        return "Not Found";
    }
}
void skiplist::displayNode() {
    for(int i = 0;i < lvl_;i++) {
        skiplistNode* node = head_->level_[i]->forward_;
        cout << "level: " << i << endl;
        while(node != NULL) {
            cout << node->score_ << " : " << node->key_ << endl;
            node = node->level_[i]->forward_;
        }
    }
}