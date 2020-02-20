#include "Server.h"
#include "DataBase.h"

using namespace dataStructure;

void Server::Init() {
  database_.emplace_back(shared_ptr<DataBase>(new DataBase()));
  dbSize_ = database_.size();
  dbIndex_ = database_.size() - 1;
  database_[dbIndex_]->rdbLoad();
  // init command table
  cmdtable_.insert(make_pair("get", std::bind(&Server::getCommand, this, _1)));
  cmdtable_.insert(make_pair("set", std::bind(&Server::setCommand, this, _1)));
  cmdtable_.insert(
      make_pair("bgsave", std::bind(&Server::bgsaveCommand, this, _1)));
  cmdtable_.insert(make_pair("del", std::bind(&Server::delCommand, this, _1)));
  cmdtable_.insert(
      make_pair("select", std::bind(&Server::selectCommand, this, _1)));
  cmdtable_.insert(
      make_pair("expire", std::bind(&Server::expireTimeCommand, this, _1)));
  cmdtable_.insert(
      make_pair("rpush", std::bind(&Server::rpushCommand, this, _1)));
  cmdtable_.insert(
      make_pair("rpop", std::bind(&Server::rpopCommand, this, _1)));
  cmdtable_.insert(
      make_pair("hset", std::bind(&Server::hsetCommand, this, _1)));
  cmdtable_.insert(
      make_pair("hget", std::bind(&Server::hgetCommand, this, _1)));
  cmdtable_.insert(
      make_pair("hgetall", std::bind(&Server::hgetallCommand, this, _1)));
  cmdtable_.insert(
      make_pair("zadd", std::bind(&Server::zaddCommand, this, _1)));
}
void Server::onConnection(const AcceptorPtr &conn) {
  std::cout << "New Connection" << std::endl;
}
void Server::onMessage(const AcceptorPtr &conn, Buffer *buf, ssize_t len) {
  std::string res = commandRequest(buf);
  std::cout << "Response has been sent: " << res << std::endl;
  conn->send(res);
}
void Server::parentHandle(int sig) {
  std::cout << "parent process-----------------" << std::endl;
}
bool Server::CheckStorageConditions() {
  long save_interval = getTimestamp() - lastsave_;
  if (save_interval > dataStructure::saveTime) {
    cout << "saveing" << endl;
    rdbSave();
    // update saved time
    lastsave_ = getTimestamp();
    return true;
  }
  return false;
}
void Server::rdbSave() {
  pid_t pid = fork();
  if (pid > 0) {
  } else if (pid == 0) {
    std::cout << "hello child" << std::endl;
    char buf[1024] = {0};
    std::string tmp = getcwd(buf, sizeof(buf));
    assert(tmp.c_str() != NULL);
    tmp += "/dump.rdb";
    std::cout << "tmp: The File Path: " << tmp << std::endl;
    std::ofstream out;
    //首先打开文件
    out.open(tmp, std::ios::app | std::ios::out | std::ios::binary);
    if (!out.is_open()) {
      cout << "文件打开失败" << endl;
      abort();
    }
    string str;
    //保存头部信息
    str = saveHead();
    for (int i = 0; i < database_.size(); i++) {
      if (!database_[i]->getKeySpaceStringObject().size() &&
          !database_[i]->getKeySpaceHashObject().size() &&
          !database_[i]->getKeySpaceListObject().size()) {
        std::cout << "The Data is Empty" << std::endl;
        return;
      }
      str += saveSelectDb(i);
      //如果字符串对象不为空便开始写入过程
      if (database_[i]->getKeySpaceStringObject().size() != 0) {
        str += saveType(ObjString);
        //首先判断字符串对象，实现对字符串对象的保存。
        auto it = database_[i]->getKeySpaceStringObject().begin();
        while (it != database_[i]->getKeySpaceStringObject().end()) {
          str += saveExpireTime(database_[i]->getKeySpaceExpiresTime(
              dataStructure::ObjString, it->first));
          str += saveKeyValue(it->first, it->second);
          it++;
        }
        str += "EOF";
        out.write(str.c_str(), str.size());
      }
      if (database_[i]->getKeySpaceHashObject().size() != 0) {
        //保存哈希对象
        str += saveType(ObjHash);
        auto it = database_[i]->getKeySpaceHashObject().begin();
        while (it != database_[i]->getKeySpaceHashObject().end()) {
          str += saveExpireTime(
              database_[i]->getKeySpaceExpiresTime(ObjHash, it->first));
          auto iter = it->second.begin();
          string tmp = "!" + to_string(it->second.size());
          while (iter != it->second.end()) {
            tmp += saveKeyValue(iter->first, iter->second);
            iter++;
          }
          str += "!" + to_string(it->first.size()) + "#" + it->first.c_str() +
                 tmp + "EOF";
          it++;
        }
        out.write(str.c_str(), str.size());
      }
      if (database_[i]->getKeySpaceListObject().size() != 0) {
        //保存列表对象
        str += saveType(ObjList);
        memset(buf, 0, sizeof(buf));
        auto it = database_[i]->getKeySpaceListObject().begin();
        while (it != database_[i]->getKeySpaceListObject().end()) {
          str += saveExpireTime(
              database_[i]->getKeySpaceExpiresTime(ObjList, it->first));
          auto iter = it->second.begin();
          string tmp = "!" + to_string(it->second.size());
          while (iter != it->second.end()) {
            tmp += "!" + to_string(iter->size()) + "$" + iter->c_str();
            iter++;
          }
          str += "!" + to_string(it->first.size()) + "#" + it->first.c_str() +
                 tmp + "EOF";
          it++;
        }
        out.write(str.c_str(), str.size());
      }
      out.close();
      exit(0);
    }
  } else
    cout << "fork error" << endl;
}
const string Server::zaddCommand(vector<string> &&argv) {
  cout << "zaddCommand" << endl;
  if (argv.size() != 4) return Status::ioError("Wrong parameter").toString();
  bool flags = database_[dbIndex_]->addKeySpace(
      dataStructure::ObjZset, dataStructure::EncodingSkipList, argv[2], argv[3],
      dataStructure::SpareTire, DefaultTime);
  if (!flags) {
    return Status::ioError("Set error").toString();
  } else {
    return Status().toString();
  }
}
const std::string Server::getCommand(vector<string> &&argv) {
  std::cout << "getCommand: " << argv[1] << std::endl;
  if (argv.size() != 2) return Status::ioError("Wrong parameter").toString();
  std::string res =
      database_[dbIndex_]->getKeySpace(dataStructure::ObjString, argv[1]);
  if (res.c_str() == NULL) {
    return Status::ioError("Empty Content").toString();
  } else
    return res + "\r\n";
}
const std::string Server::setCommand(vector<string> &&argv) {
  if (argv.size() != 3) return Status::ioError("Wrong parameter").toString();
  bool flags = database_[dbIndex_]->addKeySpace(
      dataStructure::ObjString, dataStructure::EncodingString, argv[1], argv[2],
      dataStructure::SpareTire, DefaultTime);
  std::cout << "size: " << database_[dbIndex_]->getKeySpaceStringSize()
            << std::endl;
  if (!flags) {
    return Status::ioError("Set error").toString();
  } else {
    return Status().toString();
  }
}
const std::string Server::bgsaveCommand(vector<string> &&argv) {
  if (argv.size() != 1) return Status::ioError("Wrong parameter").toString();
  bool flags = CheckStorageConditions();
  if (flags) return Status::Ok().toString();

  return Status::ioError("Persistence Error").toString();
}
const std::string Server::delCommand(vector<string> &&argv) {
  if (argv.size() != 1) return Status::ioError("Wrong parameter").toString();
  bool flags =
      database_[dbIndex_]->delKeySpace(dataStructure::ObjString, argv[0]);
  if (flags)
    return Status::Ok().toString();
  else
    return Status::ioError("Delete Error").toString();
}
const std::string Server::selectCommand(vector<string> &&argv) {
  if (argv.size() != 1) return Status::ioError("Wrong parameter").toString();
  int index = atoi(argv[1].c_str());
  database_.emplace_back(shared_ptr<DataBase>(new DataBase()));
  dbSize_ = database_.size();
  dbIndex_ = index - 1;
  return Status::Ok().toString();
}
const std::string Server::expireTimeCommand(vector<string> &&argv) {
  assert(dbIndex_ >= 0);
  if (argv.size() != 2) return Status::ioError("Wrong parameter").toString();
  vector<string> tmp;
  tmp.emplace_back(argv[1]);
  std::string value = getCommand(std::move(tmp));
  if (value == "Empty Reply") return value_;
  long long t = atoi(argv[1].c_str());
  bool flags = database_[dbIndex_]->addKeySpace(
      dataStructure::ObjString, dataStructure::EncodingString, argv[1], value,
      dataStructure::SpareTire, t);
  if (flags)
    return Status::Ok().toString();
  else
    return Status::ioError("Expire Error").toString();
}
const std::string Server::hsetCommand(vector<string> &&argv) {
  assert(dbIndex_ >= 0);
  if (argv.size() != 4) return Status::ioError("Wrong parameter").toString();
  bool flags = database_[dbIndex_]->addKeySpace(
      dataStructure::ObjHash, dataStructure::EncodingString, argv[1], argv[2],
      argv[3], DefaultTime);
  if (flags)
    return Status::Ok().toString();
  else
    return Status::ioError("Expire Error").toString();
}
const std::string Server::hgetCommand(vector<string> &&argv) {
  assert(dbIndex_ >= 0);
  if (argv.size() != 2) return Status::ioError("Wrong parameter").toString();
  std::string tmp =
      database_[dbIndex_]->getKeySpace(dataStructure::ObjHash, argv[1]);
  return tmp;
}
const std::string Server::hgetallCommand(vector<string> &&argv) {
  assert(dbIndex_ >= 0);
  if (argv.size() != 0) return Status::ioError("Wrong parameter").toString();
  auto it = database_[dbIndex_]->getKeySpaceHashObject().begin();
  std::string tmp, res;
  std::string transition;
  while (it != database_[dbIndex_]->getKeySpaceHashObject().end()) {
    tmp = database_[dbIndex_]->getKeySpace(dataStructure::ObjHash, it->first);
    transition = it->first + ": " + tmp + " ";
    res += transition;
    it++;
  }
  std::cout << "Server[hgetall]: " << res << std::endl;
  if (res.size() == 0) res = Status::notFound("Not Found hgetall").toString();
  return res;
}
const std::string Server::rpushCommand(vector<string> &&argv) {
  assert(dbIndex_ >= 0);
  if (argv.size() < 2) return Status::ioError("Wrong parameter").toString();
  std::string res = std::string();
  std::istringstream str(argv[2]);
  std::string tmp;
  bool flags;
  while (str >> tmp) {
    std::cout << "test rpush " << tmp << std::endl;
    flags = database_[dbIndex_]->addKeySpace(
        dataStructure::ObjList, dataStructure::EncodingString, argv[1], tmp,
        dataStructure::SpareTire, DefaultTime);
  }
  if (flags)
    res = Status::Ok().toString();
  else
    res = Status::ioError("Rpush Error").toString();
  return res;
}
const std::string Server::rpopCommand(vector<string> &&argv) {
  assert(dbIndex_ >= 0);
  if (argv.size() != 2) return Status::ioError("Wrong parameter").toString();
  std::string res = database_[dbIndex_]->delListObject(argv[1]);
  if (res.size() >= 0)
    return res;
  else
    return Status::ioError("Rpop Error").toString();
}
std::string Server::commandRequest(Buffer *buf) {
  std::string res = std::string();
  std::string org = buf->retrieveAllAsString();
  std::cout << "Request: " << org << std::endl;
  string dst = "\r\n";
  int ret = org.find(dst);
  cmdLen_ = atoi(org.substr(1, ret).c_str());
  int pos = org.find('$', 0);
  ret = org.find(dst.c_str(), pos, 2);
  keylen_ = atoi(org.substr(pos + 1, ret).c_str());
  cmd_ = org.substr(ret + dst.size(), keylen_);
  if (cmd_ == "get") {
    auto it = cmdtable_.find(cmd_);
    if (it == cmdtable_.end())
      res = Status::notFound("Not Found get").toString();
    else {
      pos = org.find('$', ret);
      ret = org.find(dst.c_str(), pos, 2);
      keylen_ = atoi(org.substr(pos + 1, ret).c_str());
      key_ = org.substr(ret + dst.size(), keylen_);
      vector<string> v = {cmd_,key_};
      res = it->second(std::move(v));
    }
  } else if (cmd_ == "set") {
    auto it = cmdtable_.find(cmd_);
    if (it == cmdtable_.end())
      res = Status::notFound("Not Found set").toString();
    else {
      pos = org.find('$', ret);
      ret = org.find(dst.c_str(), pos, 2);
      keylen_ = atoi(org.substr(pos + 1, ret).c_str());
      key_ = org.substr(ret + dst.size(), keylen_);
      pos = org.find('$', ret);
      ret = org.find(dst.c_str(), pos, 2);
      valuelen_ = atoi(org.substr(pos + 1, ret).c_str());
      value_ = org.substr(ret + dst.size(), valuelen_);
      vector<string> v = {cmd_,key_,value_};
      res = it->second(std::move(v));
    }
  } else if (cmd_ == "bgsave") {
    auto it = cmdtable_.find(cmd_);
    vector<string> v;
    v.emplace_back(cmd_);
    if (it == cmdtable_.end())
      res = Status::notFound("Not Found This Command bgsave").toString();
    else
      res = it->second(std::move(v));
  } else if (cmd_ == "del") {
    auto it = cmdtable_.find(cmd_);
    if (it == cmdtable_.end())
      res = Status::notFound("Not Found This Command del").toString();
    else {
      pos = org.find('$', ret);
      ret = org.find(dst.c_str(), pos, 2);
      keylen_ = atoi(org.substr(pos + 1, ret).c_str());
      key_ = org.substr(ret + dst.size(), keylen_);
      vector<string> v = {cmd_,key_};
      res = it->second(std::move(v));
    }
  } else if (cmd_ == "select") {
    auto it = cmdtable_.find(cmd_);
    if (it == cmdtable_.end())
      res = Status::notFound("Not Found This Command select").toString();
    else {
      
      pos = org.find('$', ret);
      ret = org.find(dst.c_str(), pos, 2);
      keylen_ = atoi(org.substr(pos + 1, ret).c_str());
      key_ = org.substr(ret + dst.size(), keylen_);
      vector<string> v = {cmd_,key_};
      res = it->second(std::move(v));
    }
  } else if (cmd_ == "expire") {
    auto it = cmdtable_.find(cmd_);
    if (it == cmdtable_.end())
      res = Status::notFound("Not Found This Command expire").toString();
    else {
      pos = org.find('$', ret);
      ret = org.find(dst.c_str(), pos, 2);
      keylen_ = atoi(org.substr(pos + 1, ret).c_str());
      key_ = org.substr(ret + dst.size(), keylen_);
      pos = org.find('$', ret);
      ret = org.find(dst.c_str(), pos, 2);
      valuelen_ = atoi(org.substr(pos + 1, ret).c_str());
      value_ = org.substr(ret + dst.size(), valuelen_);
      vector<string> v = {cmd_,key_,value_};
      res = it->second(std::move(v));
    }
  } else if (cmd_ == "hset") {
    auto it = cmdtable_.find(cmd_);
    if (it == cmdtable_.end())
      res = Status::notFound("Not Found This Command hset").toString();
    else {
      pos = org.find('$', ret);
      ret = org.find(dst.c_str(), pos, 2);
      keylen_ = atoi(org.substr(pos + 1, ret).c_str());
      key_ = org.substr(ret + dst.size(), keylen_);
      pos = org.find('$', ret);
      ret = org.find(dst.c_str(), pos, 2);
      skeylen_ = atoi(org.substr(pos + 1, ret).c_str());
      skey_ = org.substr(ret + dst.size(), skeylen_);
      pos = org.find('$', ret);
      ret = org.find(dst.c_str(), pos, 2);
      valuelen_ = atoi(org.substr(pos + 1, ret).c_str());
      value_ = org.substr(ret + dst.size(), valuelen_);
      vector<string> v = {cmd_,key_,skey_,value_};
      std::cout << "value: " << value_ << "size: " << value_.size()
                << std::endl;
      res = it->second(std::move(v));
    }
  } else if (cmd_ == "hget") {
    auto it = cmdtable_.find(cmd_);
    if (it == cmdtable_.end())
      res = Status::notFound("Not Found This Command hget").toString();
    else {
      pos = org.find('$', ret);
      ret = org.find(dst.c_str(), pos, 2);
      keylen_ = atoi(org.substr(pos + 1, ret).c_str());
      key_ = org.substr(ret + dst.size(), keylen_);
      vector<string> v = {cmd_,key_};      
      res = it->second(std::move(v));
    }
  } else if (cmd_ == "rpop") {
    auto it = cmdtable_.find(cmd_);
    if (it == cmdtable_.end())
      res = Status::notFound("Not Found This Command rpop").toString();
    else {
      pos = org.find('$', ret);
      ret = org.find(dst.c_str(), pos, 2);
      keylen_ = atoi(org.substr(pos + 1, ret).c_str());
      key_ = org.substr(ret + dst.size(), keylen_);
      vector<string> v = {cmd_,key_};      
      res = it->second(std::move(v));
    }
  } else if (cmd_ == "rpush") {
    auto it = cmdtable_.find(cmd_);
    if (it == cmdtable_.end())
      res = Status::notFound("Not Found This Command rpush").toString();
    else {
      pos = org.find('$', ret);
      ret = org.find(dst.c_str(), pos, 2);
      keylen_ = atoi(org.substr(pos + 1, ret).c_str());
      key_ = org.substr(ret + dst.size(), keylen_);
      pos = org.find('$', ret);
      ret = org.find(dst.c_str(), pos, 2);
      valuelen_ = atoi(org.substr(pos + 1, ret).c_str());
      value_ = org.substr(ret + dst.size(), valuelen_);
      vector<string> v = {cmd_,key_,value_};
      res = it->second(std::move(v));
    }
  } else if (cmd_ == "hgetall") {
    auto it = cmdtable_.find(cmd_);
    vector<string> v = {cmd_};
    if (it == cmdtable_.end())
      res = Status::notFound("Not Found This Command hgetall").toString();
    else
      res = it->second(std::move(v));
  } else if (cmd_ == "zadd") {
    auto it = cmdtable_.find(cmd_);
    if (it == cmdtable_.end())
      res = Status::notFound("Not Found This Command rpush").toString();
    else {
      pos = org.find('$', ret);
      ret = org.find(dst.c_str(), pos, 2);
      keylen_ = atoi(org.substr(pos + 1, ret).c_str());
      key_ = org.substr(ret + dst.size(), keylen_);
      pos = org.find('$', ret);
      ret = org.find(dst.c_str(), pos, 2);
      skeylen_ = atoi(org.substr(pos + 1, ret).c_str());
      skey_ = org.substr(ret + dst.size(), skeylen_);
      pos = org.find('$', ret);
      ret = org.find(dst.c_str(), pos, 2);
      valuelen_ = atoi(org.substr(pos + 1, ret).c_str());
      value_ = org.substr(ret + dst.size(), valuelen_);
      vector<string> v = {cmd_,key_,skey_,value_};
      res = it->second(std::move(v));
    }
  } else {
    std::cout << "[Server::commandRequest:] No Command " << std::endl;
    res = Status::invalidArgument("No Command").toString();
  }
  return res;
}