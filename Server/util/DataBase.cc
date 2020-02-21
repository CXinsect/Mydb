#include "DataBase.h"

bool DataBase::addKeySpace(int type, int encoding, const std::string &key,
                           const std::string &value, const std::string &value1,
                           long long expiresTime) {
  int tempTime = expiresTime + getTimestamp();
  if (type == dataStructure::ObjString) {
    SMap::iterator Siter = sMap_.find(key);
    if (Siter == sMap_.end()) {
      String_.insert(make_pair(key, value));
      sMap_.insert(make_pair(key, tempTime));
      stringLru_.set(key, value);
    } else {
      String_.erase(key);
      sMap_.erase(key);
      sMap_.insert(make_pair(key, tempTime));
      String_.insert(make_pair(key, value));
      stringLru_.set(key, value);
    }
  } else if (type == dataStructure::ObjHash) {
    HMap::iterator Hiter = hMap_.find(key);
    if (Hiter == hMap_.end()) {
      mutiMapPool<string,string> tmp;
      tmp.insert(make_pair(value, value1));
      Hash_.insert(make_pair(key, tmp));
      hMap_.insert(make_pair(key, tempTime));
      hashLru_.set(key, tmp);
      cout << "hash_key::::" << key << endl;
    } else {
      // std::multimap<std::string, std::string> tmp;
      mutiMapPool<string,string> tmp;
      Hash::iterator it = Hash_.find(key);
      auto iter = it->second.begin();
      while (iter != it->second.end()) {
        tmp.insert(make_pair(iter->first, iter->second));
        iter++;
      }
      tmp.insert(make_pair(value, value1));
      std::cout << "rmp: " << tmp.size() << std::endl;
      Hash_.erase(key);
      hMap_.erase(key);
      Hash_.insert(make_pair(key, tmp));
      hMap_.insert(make_pair(key, tempTime));
      hashLru_.set(key, tmp);
      cout << "hash_key::::" << key << endl;
    }
  } else if (type == dataStructure::ObjList) {
    LMap::iterator lIter = lMap_.find(key);
    if (lIter == lMap_.end()) {
      // std::list<std::string> tmp;
      listPool<string> tmp;
      tmp.push_back(value);
      List_.insert(make_pair(key, tmp));
      lMap_.insert(make_pair(key, tempTime));
      listLRu_.set(key, tmp);
    } else {
      //更新list的值
      auto it = List_.find(key);
      std::list<std::string>::iterator iter = it->second.begin();
      // std::list<std::string> tmp;
      listPool<string> tmp;
      while (iter != it->second.end()) {
        tmp.push_back(*iter);
        iter++;
      }
      tmp.push_back(value);
      List_.erase(key);
      lMap_.erase(key);
      List_.insert(make_pair(key, tmp));
      lMap_.insert(make_pair(key, tempTime));
      listLRu_.set(key, tmp);
    }
  } else if (type == dataStructure::ObjZset) {
    skip_->insertNode(atoi(key.c_str()), value);
  } else {
    std::cout << "Unknown type" << std::endl;
    return false;
  }
  std::cout << "addKeySpace has been Executed" << std::endl;
  return true;
}
bool DataBase::delKeySpace(int type, const std::string &key) {
  if (type == dataStructure::ObjString) {
    SMap ::iterator hIter = sMap_.find(key);
    if (hIter != sMap_.end()) {
      String_.erase(key);
      return true;
    } else {
      cout << "Not Found " << endl;
    }
  } else if (type == dataStructure::ObjHash) {
    HMap::iterator hIter = hMap_.find(key);
    if (hIter != hMap_.end()) {
      Hash_.erase(key);
      return true;
    }
  } else if (type == dataStructure::ObjList) {
    LMap::iterator lIter = lMap_.find(key);
    if (lIter != lMap_.end()) {
      List_.erase(key);
      return true;
    }
  } else {
    std::cout << "Unknown type" << std::endl;
    abort();
  }
  return false;
}
const std::string DataBase::delListObject(const std::string &key) {
  auto lIter = lMap_.find(key);
  if (lIter != lMap_.end()) {
    auto it = List_.find(key);
    if (it != List_.end()) {
      std::string res = it->second.back();
      cout << "res: " << res << key << endl;
      it->second.pop_back();
      return res;
    } else {
      return Status::notFound("Not Found key | has been deleted").toString();
    }
  } else {
    return Status::notFound("Not Found key").toString();
  }
}
std::string DataBase::getKeySpace(int type, const std::string &key) {
  std::string ret = std::string();
  if (!judgeKeySpaceExpiresTime(type, key)) {
    if (type == dataStructure::ObjString) {
      auto sIter = String_.find(key);
      if (sIter == String_.end()) {
        ret = Status::notFound("Not Found").toString();
      } else {
        auto it = String_.find(key);
        ret = '+' + it->second;
      }
    } else if (type == dataStructure::ObjHash) {
      auto hIter = Hash_.find(key);
      if (hIter == Hash_.end()) {
        std::cout << "Not Found" << std::endl;
        cout << "hash__key: " << key << endl;
        ret = Status::notFound("Not Found").toString();

      } else {
        char buf[1024] = {0};
        char *pbuf = buf;
        auto it = Hash_.find(key);
        int n = 0, len = 0;
        for (auto iter = it->second.begin(); iter != it->second.end();
             iter = it->second.upper_bound(iter->first)) {
          auto pos = it->second.equal_range(iter->first);
          for (auto i = pos.first; i != pos.second; i++) {
            std::cout << "test " << i->first << ": " << i->second << std::endl;
            n = snprintf(pbuf, sizeof(buf) - len, "%s %s ", i->first.c_str(),
                         i->second.c_str());
            pbuf += n;
            len += n;
          }
        }
        ret = buf;
        string t = "+";
        ret = t + ret;
        cout << ret << endl;
      }
    } else if (type == dataStructure::ObjList) {
      auto lIter = List_.find(key);
      if (lIter == List_.end()) {
        std::cout << "Not Found" << std::endl;
        cout << "hash__key: " << key << endl;
        ret = Status::notFound("Not Found").toString();
      } else {
        auto it = List_.find(key);
        std::list<std::string>::iterator iter = it->second.begin();
        ret = '+' + *iter;
      }
    } else {
      std::cout << "Unknown Type" << std::endl;
      ret = Status::invalidArgument("InvaildArgument").toString();
    }
  } else {
    delKeySpace(type, key);
    ret = "The key has expired and has been deleted";
  }
  return ret;
}
const string DataBase::getSkiplistCount(rangeSpec& range) {
  return to_string(skip_->getCountRange(range));
}
long long DataBase::getKeySpaceExpiresTime(int type, const std::string &key) {
  long long ret;
  if (type == dataStructure::ObjString) {
    auto it = sMap_.find(key);
    if (it == sMap_.end())
      return -1;
    else
      ret = it->second;
  } else if (type == dataStructure::ObjHash) {
    auto it = hMap_.find(key);
    if (it == hMap_.end())
      return -3;
    else
      ret = it->second;
  } else if (type == dataStructure::ObjList) {
    auto it = lMap_.find(key);
    if (it == lMap_.end())
      ret = -2;
    else
      ret = it->second;
  } else {
    ret = -3;
  }
  return ret;
}

long long DataBase::RemainingSurvivalTime(int type, const std::string &key) {
  long long ret = getKeySpaceExpiresTime(type, key);
  assert(ret != -1);
  long long now = getTimestamp();
  return now - ret;
}
bool DataBase::judgeKeySpaceExpiresTime(int type, const std::string &key) {
  long long expire = getKeySpaceExpiresTime(type, key);
  if (expire <= -1)
    return false;
  else {
    long long now = getTimestamp();
    if (now > expire)
      return true;
    else
      return false;
  }
}
void DataBase::rdbLoad() {
  char buf[2 * 1024] = {0};
  std::string path = getcwd(buf, sizeof(buf));
  path += "/dump.rdb";
  int fd = open(path.c_str(), O_CREAT | O_RDWR, 0644);
  cout << strerror(errno) << endl;
  assert(fd != -1);
  struct stat stat_;
  int ret = ::stat(path.c_str(), &stat_);
  assert(ret != -1);
  if (stat_.st_size == 0) return;
  char *addr = static_cast<char *>(
      mmap(NULL, stat_.st_size, PROT_READ, MAP_SHARED, fd, 0));
  if (addr == MAP_FAILED) {
    cout << "Mmap Error" << endl;
    abort();
  }
  close(fd);
  if (strlen(buf) == 0) return;
  string data(addr, addr + stat_.st_size);
  assert(munmap(addr, stat_.st_size) != -1);
  std::cout << "data: " << data << data.size() << std::endl;
  int pos;
  ret = data.find("FE");
  pos = data.find('^', ret);
  cout << pos << ret << endl;
  int dbNum = atoi(InterceptString(data, ret + 2, pos).c_str());
  ret = data.find("FD");
  assert(ret != -1);
  int type = atoi(InterceptString(data, pos + 1, ret).c_str());
  cout << dbNum << type << endl;
  pos = data.find('!', ret);
  long long expireTime = atoi(InterceptString(data, ret + 2, pos).c_str());
  int end = data.find("EOF");
  while (pos < end && ret < end) {
    if (type == ObjString) {
      ret = data.find('#', pos);
      int mkLen = atoi(InterceptString(data, pos + 1, ret).c_str());
      string key = data.substr(ret + 1, mkLen);
      pos = data.find('!', ret);
      ret = data.find('$', pos);
      int valueLen = atoi(InterceptString(data, pos + 1, ret).c_str());
      string value = data.substr(ret + 1, valueLen);
      String_.insert(make_pair(key, value));
      ret += 1 + valueLen;
      continue;
    }
    if (type == ObjHash) {
      // std::multimap<std::string, std::string> tmp;
      mutiMapPool<string,string> tmp;
      ret = data.find('#', pos);
      int mkLen = atoi(InterceptString(data, pos + 1, ret).c_str());
      string key = data.substr(ret + 1, mkLen);
      pos = data.find('!', ret);
      // Skip hash size
      ret = data.find('!', pos + 1);
      int num = atoi(InterceptString(data, pos + 1, ret).c_str());
      int valueLen = 0;
      while (num > 0) {
        pos = data.find('#', ret);
        int skeyLen = atoi(InterceptString(data, ret + 1, pos).c_str());
        string skey = data.substr(pos + 1, skeyLen);
        ret = data.find('!', pos);
        pos = data.find('$', ret);
        valueLen = atoi(InterceptString(data, ret + 1, pos).c_str());
        string value = data.substr(pos + 1, valueLen);
        tmp.insert(make_pair(skey, value));
        num--;
      }
      pos += 1 + valueLen;

      Hash_.insert(make_pair(key, tmp));
      continue;
    }
    if (type == ObjList) {
      ret = data.find_first_of('#');
      int mkLen = atoi(InterceptString(data, pos + 1, ret).c_str());
      string key = data.substr(ret + 1, mkLen);
      pos = data.find('!', ret);
      ret = data.find('!', pos + 1);
      int listSize = atoi(InterceptString(data, pos + 1, ret).c_str());
      // std::list<std::string> tmp;
      listPool<string> tmp;
      int valueLen = 0;
      while (listSize-- > 0) {
        pos = data.find('$', ret);
        valueLen = atoi(InterceptString(data, ret + 1, pos).c_str());
        string value = data.substr(pos + 1, valueLen);
        tmp.push_back(value);
        ret = data.find('!', pos);
      }
      if (ret == -1) break;
      ret += 1 + valueLen;
      List_.insert(make_pair(key, tmp));
      continue;
    }
    std::cout << "Finally: " << data << std::endl;
  }
}