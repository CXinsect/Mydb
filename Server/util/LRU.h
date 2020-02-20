#ifndef __LRU_H_
#define __LRU_H_
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

using namespace std;
template <typename T1, typename T2>
struct LinkedNode {
  T1 key;
  T2 value;
  LinkedNode<T1, T2>* pre;
  LinkedNode<T1, T2>* next;
};

template <typename T1, typename T2>
class LRUCache {
 public:
  LRUCache() : capicaty(1000) {
    head = new LinkedNode<T1, T2>;
    tail = new LinkedNode<T1, T2>;
    head->next = tail;
    tail->pre = head;
  }
  LRUCache(int cap) : count(0), capicaty(cap), head(NULL), tail(NULL) {
    head = new LinkedNode<T1, T2>;
    tail = new LinkedNode<T1, T2>;
    head->next = tail;
    tail->pre = head;
  }
  int get(T1 key) {
    auto ret = cache.find(key);
    if (ret != cache.end()) {
      moveToHead(ret->second);
      return ret->second->value;
    } else
      return 0;
  }
  void set(T1 key, T2 value) {
    auto ret = cache.find(key);

    if (ret == cache.end()) {
      LinkedNode<T1, T2>* newNode(new LinkedNode<T1, T2>);
      newNode->key = key;
      newNode->value = value;
      cache.insert(make_pair(key, newNode));
      addNode(newNode);
      count++;
      if (count > capicaty) {
        LinkedNode<T1, T2> tail = popTail();
        cache.erase(tail.key);
        count--;
      }
    } else {
      ret->second->value = value;
      moveToHead(ret->second);
    }
  }
  int getCount() { return count; }
  // ~LRUCache() {
  //     while(head != NULL) {
  //         LinkedNode<T>* tmp = head->next;
  //         delete head;
  //         head = tmp;
  //     }
  // }
 private:
  void addNode(LinkedNode<T1, T2>* node) {
    node->pre = head;
    node->next = head->next;
    head->next->pre = node;
    head->next = node;
  }

  void removeNode(LinkedNode<T1, T2>* node) {
    // LinkedNode<T>* pre = node->pre;
    // LinkedNode<T>* next = node->next;
    // pre->next = next;
    // next->pre = pre;
    node->pre->next = node->next;
    node->next->pre = node->pre;
  }
  void moveToHead(LinkedNode<T1, T2>* node) {
    if (node != NULL) {
      removeNode(node);
      addNode(node);
    }
  }

  LinkedNode<T1, T2> popTail() {
    LinkedNode<T1, T2>* res = tail->pre;
    removeNode(res);
    return *res;
  }

 private:
  unordered_map<T1, LinkedNode<T1, T2>*> cache;
  int count;
  int capicaty;
  LinkedNode<T1, T2>*head, *tail;
};

#endif