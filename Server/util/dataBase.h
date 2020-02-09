
#ifndef _DATABASE_H_
#define _DATABASE_H_

#include "modelHead.h"
#include "status.h"
#include "LRU.h"
#include <unordered_map>

class redisPersistence;

using namespace std;
using namespace Mydb;

class DataBase
{
public:
  DataBase(){};
  ~DataBase() {}

public:
  typedef unordered_map<std::string, std::string> String;
  typedef unordered_map<std::string, std::multimap<std::string, std::string>> Hash;
  typedef unordered_map<std::string, std::list<std::string>> List;
  const long long DefaultTime = -2038;

  bool addKeySpace(int type, int encoding, const std::string &key,
                   const std::string &value, const std::string &value1,
                   long long expiresTime);
  bool delKeySpace(int type, const std::string &key);
  const std::string delListObject(const std::string &key);
  std::string getKeySpace(int type, const std::string &key);

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

private:
  long long getTimestamp()
  {
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

private:
  //过期时间
  typedef unordered_map<string, long long> SMap;
  typedef unordered_map<string, long long> HMap;
  typedef unordered_map<string, long long> LMap;

  SMap sMap_;
  HMap hMap_;
  LMap lMap_;

  LRUCache<string, string> stringLru_;
  LRUCache<string, multimap<string, string>> hashLru_;
  LRUCache<string, list<string>> listLRu_;
};
#endif