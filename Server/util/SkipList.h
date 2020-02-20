#ifndef __SKIPLIST_H_
#define __SKIPLIST_H_
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;
class skiplistNode;
class skiplistLevel {
 public:
  skiplistLevel() : forward_(nullptr), span_(0) {}

 public:
  skiplistNode* forward_;
  unsigned int span_;
};

class skiplistNode {
 public:
  skiplistNode(double score, const string& key, int level) {
    this->score_ = score;
    this->key_ = key;
    level_ = unique_ptr<unique_ptr<skiplistLevel>[]>(
        new unique_ptr<skiplistLevel>[level + 1]);
    for (int i = 0; i < level; i++) {
      level_[i] = unique_ptr<skiplistLevel>(new skiplistLevel);
      memset(&*level_[i], 0, sizeof(skiplistLevel));
    }
    this->back_ = nullptr;
  }

 public:
  unique_ptr<unique_ptr<skiplistLevel>[]> level_;
  double score_;
  string key_;
  skiplistNode* back_;
};
struct rangeSpec {
  rangeSpec(double min, double max)
      : min_(min), max_(max), minex_(true), maxex_(true) {}

  double min_, max_;
  //判断开闭区间
  bool minex_, maxex_;
};
class skiplist {
 public:
  skiplist(int max, float p)
      : maxlvl_(max),
        p_(p),
        lvl_(1),
        length_(0),
        head_(new skiplistNode(-1, "", max)),
        tail_(nullptr) {}
  ~skiplist() {}
  int randomLevel();
  skiplistNode* creatNode(double score, const string& key, int level);
  void insertNode(double score, const string& key);
  void deleteNode(double score, const string& key);
  const string getNodeValue(double score);
  int unsigned getNodeRank(double score, const string& key);
  bool isInRange(rangeSpec&);
  skiplistNode* getNodeFirstRange(rangeSpec&);
  skiplistNode* getNodeLastRange(rangeSpec&);
  void deleteNodeRange(rangeSpec&);
  void displayNode();
  int getLength() { return length_; }
  int getCountRange(rangeSpec&);
  vector<skiplistNode*> getNodeRange(rangeSpec&);

 private:
  //是否大于最小范围
  int valueBmin(double value, rangeSpec& range) {
    return range.minex_ ? (value > range.min_) : (value >= range.min_);
  }
  //是否小于最大范围
  int valueSmax(double value, rangeSpec& range) {
    return range.maxex_ ? (value < range.max_) : (value <= range.max_);
  }

 private:
  int maxlvl_;
  float p_;
  int lvl_;
  int length_;
  skiplistNode *head_, *tail_;
};
#endif