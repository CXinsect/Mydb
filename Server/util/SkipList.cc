#include "SkipList.h"

int skiplist::randomLevel() {
  float r = (float)rand() / RAND_MAX;
  int level = 0;
  while (r < p_ && level < maxlvl_) {
    level++;
    r = (float)rand() / RAND_MAX;
  }
  return level;
}

skiplistNode* skiplist::creatNode(double score, const string& key, int level) {
  skiplistNode* node = new skiplistNode(score, key, level);
  return node;
}

void skiplist::insertNode(double score, const string& key) {
  skiplistNode* cur = head_;
  skiplistNode* update[maxlvl_ + 1];
  memset(update, 0, sizeof(skiplistNode*) * (maxlvl_ + 1));
  unsigned int rank[maxlvl_];
  //从顶层链表向下层遍历，查找插入位置
  for (int i = lvl_ - 1; i >= 0; i--) {
    if (i == lvl_ - 1) {
      rank[i] = 0;
    } else {
      rank[i] = rank[i + 1];
    }
    while (cur->level_[i]->forward_ != NULL &&
           (cur->level_[i]->forward_->score_ < score ||
            (cur->level_[i]->forward_->score_ == score &&
             cur->level_[i]->forward_->key_ != key))) {
      rank[i] += cur->level_[i]->span_;
      cur = cur->level_[i]->forward_;
    }
    update[i] = cur;
  }
  cur = cur->level_[0]->forward_;
  if (cur == NULL || cur->score_ != score) {
    int rlevel = randomLevel();
    //如果新节点层数高于现有节点则更新指针信息
    if (rlevel > lvl_) {
      for (int i = lvl_; i < rlevel; i++) {
        rank[i] = 0;
        update[i] = head_;
        update[i]->level_[i]->span_ = length_;
      }
      lvl_ = rlevel;
    }
    //链表插入过程
    skiplistNode* node = creatNode(score, key, lvl_);
    for (int i = 0; i < lvl_; i++) {
      node->level_[i]->forward_ = update[i]->level_[i]->forward_;
      update[i]->level_[i]->forward_ = node;
      node->level_[i]->span_ =
          update[i]->level_[i]->span_ - (rank[0] - rank[i]);
      update[i]->level_[i]->span_ = (rank[0] - rank[i]) + 1;
    }
    for (int i = lvl_; i < rlevel; i++) {
      update[i]->level_[i]->span_++;
    }
    //将后退指针指向前一个节点
    node->back_ = (update[0] == head_) ? NULL : update[0];
    if (node->level_[0]->forward_)
      node->level_[0]->forward_->back_ = cur;
    else
      tail_ = node;
    length_++;
  }
}

void skiplist::deleteNode(double score, const string& key) {
  skiplistNode* cur = head_;
  skiplistNode* update[maxlvl_ + 1];
  memset(update, 0, sizeof(skiplistNode*) * (maxlvl_ + 1));
  //遍历过程
  for (int i = lvl_ - 1; i >= 0; i--) {
    while (cur->level_[i]->forward_ != NULL &&
           (cur->level_[i]->forward_->score_ < score ||
            (cur->level_[i]->forward_->score_ == score &&
             cur->level_[i]->forward_->key_ != key))) {
      cur = cur->level_[i]->forward_;
    }
    update[i] = cur;
  }
  cur = cur->level_[0]->forward_;
  if (cur && cur->score_ == score && cur->key_ == key) {
    for (int i = 0; i < lvl_; i++) {
      if (update[i]->level_[i]->forward_ != cur) {
        update[i]->level_[i]->span_ -= 1;
      } else {
        update[i]->level_[i]->span_ += cur->level_[i]->span_ - 1;
        update[i]->level_[i]->forward_ = cur->level_[i]->forward_;
      }
    }
    if (cur->level_[0]->forward_) {
      cur->level_[0]->forward_->back_ = cur->back_;
    } else {
      tail_ = cur->back_;
    }
    delete cur;
    while (lvl_ > 1 && head_->level_[--lvl_]->forward_ == NULL) {
      lvl_--;
    }
    length_--;
  }
}
const string skiplist::getNodeValue(double score) {
  skiplistNode* cur = head_;
  for (int i = lvl_ - 1; i >= 0; i--) {
    while (cur->level_[i]->forward_ &&
           cur->level_[i]->forward_->score_ < score) {
      cur = cur->level_[i]->forward_;
    }
  }
  cur = cur->level_[0]->forward_;
  if (cur->score_ == score) {
    return cur->key_;
  } else {
    return "Not Found";
  }
}
void skiplist::displayNode() {
  for (int i = 0; i < lvl_; i++) {
    skiplistNode* node = head_->level_[i]->forward_;
    cout << "level: " << i << endl;
    while (node != NULL) {
      cout << node->score_ << " : " << node->key_ << endl;
      node = node->level_[i]->forward_;
    }
  }
}
//获得节点排位
unsigned int skiplist::getNodeRank(double score, const string& key) {
  skiplistNode* cur = head_;
  unsigned long rank = 0;
  for (int i = lvl_ - 1; i >= 0; i--) {
    while (cur->level_[i]->forward_ != NULL &&
           (cur->level_[i]->forward_->score_ < score ||
            (cur->level_[i]->forward_->score_ == score &&
             cur->level_[i]->forward_->key_ != key))) {
      rank += cur->level_[i]->span_;
      cur = cur->level_[i]->forward_;
    }
    if (cur && cur->key_ == key) {
      return rank;
    }
  }
  return 0;
}
//判断范围是否合法
bool skiplist::isInRange(rangeSpec& range) {
  if (range.min_ > range.max_ ||
      (range.min_ == range.max_ && (range.minex_ || range.maxex_)))
    return false;
  skiplistNode* cur = tail_;
  if (cur == NULL || !valueBmin(cur->score_, range)) return false;
  cur = head_->level_[0]->forward_;
  if (cur == NULL || !valueSmax(cur->score_, range)) return false;
  return true;
}
//获得满足范围的第一个节点
skiplistNode* skiplist::getNodeFirstRange(rangeSpec& range) {
  skiplistNode* cur = head_;
  if (!isInRange(range)) return NULL;
  for (int i = lvl_ - 1; i >= 0; i--) {
    while (cur->level_[i]->forward_ != NULL &&
           !valueBmin(cur->level_[i]->forward_->score_, range))
      cur = cur->level_[i]->forward_;
  }
  cur = cur->level_[0]->forward_;
  if (!valueSmax(cur->score_, range)) return NULL;
  return cur;
}
//获得满足范围的最后一个节点
skiplistNode* skiplist::getNodeLastRange(rangeSpec& range) {
  skiplistNode* cur = head_;
  if (!isInRange(range)) return NULL;
  for (int i = lvl_ - 1; i >= 0; i--) {
    while (cur->level_[i]->forward_ &&
           valueSmax(cur->level_[i]->forward_->score_, range))
      cur = cur->level_[i]->forward_;
  }
  if (!valueBmin(cur->score_, range)) return NULL;
  return cur;
}
int skiplist::getCountRange(rangeSpec& range) {
  skiplistNode* cur = head_;
  skiplistNode* update[maxlvl_ + 1];
  unsigned int count = 0;
  //寻找左边界
  for (int i = lvl_ - 1; i >= 0; i--) {
    while (cur->level_[i]->forward_ &&
           (range.minex_ ? cur->level_[i]->forward_->score_ <= range.min_
                         : cur->level_[i]->forward_->score_ < range.min_))
      cur = cur->level_[i]->forward_;
    update[i] = cur;
  }
  cur = cur->level_[0]->forward_;
  //寻找右边界
  while (cur && (range.maxex_ ? cur->score_ < range.max_
                              : cur->score_ <= range.max_)) {
    count++;
    cur = cur->level_[0]->forward_;
  }
  return count;
}
vector<skiplistNode*> skiplist::getNodeRange(rangeSpec& range) {
  skiplistNode* cur = head_;
  skiplistNode* update[maxlvl_ + 1];
  vector<skiplistNode*> ret;
  ret.clear();
  //寻找左边界
  for (int i = lvl_ - 1; i >= 0; i--) {
    while (cur->level_[i]->forward_ &&
           (range.minex_ ? cur->level_[i]->forward_->score_ <= range.min_
                         : cur->level_[i]->forward_->score_ < range.min_))
      cur = cur->level_[i]->forward_;
    update[i] = cur;
  }
  cur = cur->level_[0]->forward_;
  //寻找右边界
  while (cur && (range.maxex_ ? cur->score_ < range.max_
                              : cur->score_ <= range.max_)) {
    ret.push_back(cur);
    cur = cur->level_[0]->forward_;
  }
  return ret;
}
void skiplist::deleteNodeRange(rangeSpec& range) {
  skiplistNode* cur = head_;
  skiplistNode* update[maxlvl_ + 1];
  //寻找左边界
  for (int i = lvl_ - 1; i >= 0; i--) {
    while (cur->level_[i]->forward_ &&
           (range.minex_ ? cur->level_[i]->forward_->score_ <= range.min_
                         : cur->level_[i]->forward_->score_ < range.min_))
      cur = cur->level_[i]->forward_;
    update[i] = cur;
  }
  cur = cur->level_[0]->forward_;
  //寻找右边界
  while (cur && (range.maxex_ ? cur->score_ < range.max_
                              : cur->score_ <= range.max_)) {
    //删除节点
    skiplistNode* tmp = cur->level_[0]->forward_;
    for (int i = 0; i < lvl_; i++) {
      if (update[i]->level_[i]->forward_ != cur) {
        update[i]->level_[i]->span_ -= 1;
      } else {
        update[i]->level_[i]->span_ += cur->level_[i]->span_ - 1;
        update[i]->level_[i]->forward_ = cur->level_[i]->forward_;
      }
    }
    if (cur->level_[0]->forward_) {
      cur->level_[0]->forward_->back_ = cur->back_;
    } else {
      tail_ = cur->back_;
    }
    delete cur;
    while (lvl_ > 1 && head_->level_[lvl_ - 1]->forward_ == NULL) lvl_--;
    length_--;
    cur = tmp;
  }
}
