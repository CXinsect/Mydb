
#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <sys/mman.h>
#include <unordered_map>
#include <ext/pool_allocator.h>

#include "LRU.h"
#include "ModelHead.h"
#include "SkipList.h"
#include "Status.h"

using namespace std;
using namespace Mydb;
using namespace dataStructure;

template <typename Key,typename T>
using stringPool = unordered_map<Key,T,std::hash<Key>,std::equal_to<Key>,__gnu_cxx::__pool_alloc<pair<const Key,T>>>;
template <typename Key,typename T>
using mutiMapPool = multimap<Key,T,less<Key>,__gnu_cxx::__pool_alloc<pair<const Key,T>>>;
template <typename T>
using listPool = list<T,__gnu_cxx::__pool_alloc<T>>;

class DataBase {
 public:
  DataBase() : skip_(new skiplist(8, 0.25)){};
  ~DataBase() {}

 public:
  typedef stringPool<string,string> String;
  typedef stringPool<string,mutiMapPool<string,string>> Hash;
  typedef stringPool<string,listPool<string>> List;
  const long long DefaultTime = -2038;

  bool addKeySpace(int type, int encoding, const std::string &key,
                   const std::string &value, const std::string &value1,
                   long long expiresTime);
  bool delKeySpace(int type, const std::string &key);
  const std::string delListObject(const std::string &key);
  std::string getKeySpace(int type, const std::string &key);
  const string getSkiplistCount(rangeSpec&);
  long long getKeySpaceExpiresTime(int type, const std::string &key);
  bool judgeKeySpaceExpiresTime(int type, const std::string &key);
  void deleteKeySpaceExpireTime(int type, const std::string &key);
  long long RemainingSurvivalTime(int type, const std::string &key);

 public:
  String &getKeySpaceStringObject() { return String_; }
  Hash &getKeySpaceHashObject() { return Hash_; }
  List &getKeySpaceListObject() { return List_; }
  int getKeySpaceStringSize() { return String_.size(); }
  int getKeySpaceHashSize() { return Hash_.size(); }
  int getKeySpaceListSize() { return List_.size(); }
  void rdbLoad();
  inline std::string InterceptString(const std::string &ptr, int pos1,
                                     int pos2) {
    if (pos1 > pos2) std::swap(pos1, pos2);
    int gap = pos2 - pos1;
    std::string ret = ptr.substr(pos1, gap);
    return ret;
  }

 private:
  long long getTimestamp() {
    struct timeval tv;
    assert(gettimeofday(&tv, NULL) != -1);
    return tv.tv_sec;
  }

 private:
  int size = -1;
  //键空间中的实际对象
  String String_;
  Hash Hash_;
  List List_;
  shared_ptr<skiplist> skip_;

 private:
  //过期时间
  typedef stringPool<string,long long> SMap;
  typedef stringPool<string,long long> HMap;
  typedef stringPool<string,long long> LMap;


  SMap sMap_;
  HMap hMap_;
  LMap lMap_;

  LRUCache<string, string> stringLru_;
  LRUCache<string,mutiMapPool<string,string>> hashLru_;
  LRUCache<string,listPool<string>> listLRu_;
};
#endif