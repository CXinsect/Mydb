#include "Server.h"
#include "dataBase.h"

using namespace DataStructure;

void Server::Init()
{
    database_.push_back(shared_ptr<DataBase>(new DataBase()));
    dbSize_ = database_.size();
    dbIndex_ = database_.size() - 1;
    database_[dbIndex_]->rdbLoad();
    //init command table
    cmdtable_.insert(make_pair("get", std::bind(&Server::getCommand, this, _1)));
    cmdtable_.insert(make_pair("set", std::bind(&Server::setCommand, this, _1)));
    cmdtable_.insert(make_pair("bgsave", std::bind(&Server::bgsaveCommand, this, _1)));
    cmdtable_.insert(make_pair("del", std::bind(&Server::delCommand, this, _1)));
    cmdtable_.insert(make_pair("select", std::bind(&Server::selectCommand, this, _1)));
    cmdtable_.insert(make_pair("expire", std::bind(&Server::expireTimeCommand, this, _1)));
    cmdtable_.insert(make_pair("rpush", std::bind(&Server::rpushCommand, this, _1)));
    cmdtable_.insert(make_pair("rpop", std::bind(&Server::rpopCommand, this, _1)));
    cmdtable_.insert(make_pair("hset", std::bind(&Server::hsetCommand, this, _1)));
    cmdtable_.insert(make_pair("hget", std::bind(&Server::hgetCommand, this, _1)));
    cmdtable_.insert(make_pair("hgetall", std::bind(&Server::hgetallCommand, this, _1)));
    cmdtable_.insert(make_pair("zadd",std::bind(&Server::zaddCommand, this, _1)));
}
void Server::onConnection(const AcceptorPtr &conn)
{
    std::cout << "New Connection" << std::endl;
}
void Server::onMessage(const AcceptorPtr &conn, Buffer *buf, ssize_t len)
{
    std::string res = commandRequest(buf);
    std::cout << "Response has been sent: " << res << std::endl;
    conn->send(res);
}
void Server::parentHandle(int sig)
{
    std::cout << "parent process-----------------" << std::endl;
}
bool Server::CheckStorageConditions()
{
    long save_interval = getTimestamp() - lastsave_;
    if (save_interval > DataStructure::saveTime)
    {
        cout << "saveing" << endl;
        rdbSave();
        //update saved time
        lastsave_ = getTimestamp();
        return true;
    }
    cout << "未过期" << endl;
    return false;
}
void Server::rdbSave()
{
    pid_t pid = fork();
    if (pid > 0){
    }
    else if (pid == 0)
    {
        std::cout << "hello child" << std::endl;
        char buf[1024] = {0};
        std::string tmp = getcwd(buf, sizeof(buf));
        assert(tmp.c_str() != NULL);
        tmp += "/1.rdb";
        std::cout << "tmp: The File Path: " << tmp << std::endl;
        std::ofstream out;
        //首先打开文件
        out.open(tmp, std::ios::app | std::ios::out | std::ios::binary);
        if (!out.is_open())
        {
            cout << "文件打开失败" << endl;
            abort();
        }
        string str;
        //保存头部信息
        str = saveHead();
        for (int i = 0; i < database_.size(); i++)
        {
            if (!database_[i]->getKeySpaceStringObject().size() &&
                !database_[i]->getKeySpaceHashObject().size() &&
                !database_[i]->getKeySpaceListObject().size())
            {
                std::cout << "The Data is Empty" << std::endl;
                return;
            }
            str += saveSelectDb(i);
            //如果字符串对象不为空便开始写入过程
            if (database_[i]->getKeySpaceStringObject().size() != 0)
            {
                str += saveType(ObjString);
                //首先判断字符串对象，实现对字符串对象的保存。
                auto it = database_[i]->getKeySpaceStringObject().begin();
                while (it != database_[i]->getKeySpaceStringObject().end())
                {
                    str += saveExpireTime(database_[i]->getKeySpaceExpiresTime(DataStructure::ObjString, it->first));
                    str += saveKeyValue(it->first,it->second);
                    it++;
                }
                str += "EOF";
                out.write(str.c_str(),str.size());
            }
            if (database_[i]->getKeySpaceHashObject().size() != 0)
            {
                //保存哈希对象
                str += saveType(ObjHash);
                auto it = database_[i]->getKeySpaceHashObject().begin();
                while (it != database_[i]->getKeySpaceHashObject().end())
                {
                    str += saveExpireTime(database_[i]->getKeySpaceExpiresTime(ObjHash,it->first));
                    auto iter = it->second.begin();
                    string tmp = "!" + to_string(it->second.size());
                    while (iter != it->second.end())
                    {
                        tmp += saveKeyValue(iter->first,iter->second);
                        iter++;
                    }
                    str += "!" + to_string(it->first.size()) + "#" + it->first.c_str() + tmp + "EOF";    
                    it++;
                }
                out.write(str.c_str(),str.size());
            }
            if (database_[i]->getKeySpaceListObject().size() != 0)
            {
                //保存列表对象
                str += saveType(ObjList);
                memset(buf, 0, sizeof(buf));
                auto it = database_[i]->getKeySpaceListObject().begin();
                while (it != database_[i]->getKeySpaceListObject().end())
                {   
                    str += saveExpireTime(database_[i]->getKeySpaceExpiresTime(ObjList,it->first));
                    auto iter = it->second.begin();
                    string tmp = "!" + to_string(it->second.size());
                    while (iter != it->second.end())
                    {
                        tmp += "!" + to_string(iter->size()) + "$" + iter->c_str();
                        iter++;
                    }
                    str += "!" + to_string(it->first.size()) + "#" + it->first.c_str() + tmp + "EOF";
                    it++;
                }
                out.write(str.c_str(),str.size());
            }
            out.close();
            exit(0);
        }
    }
    else
        cout << "fork error" << endl;
}
const string Server::zaddCommand(const vector<string>& argv) {
    cout << "zaddCommand" << endl;
    if(argv.size() != 4) return Status::IOError("Wrong parameter").ToString();
    bool flags = database_[dbIndex_]->addKeySpace(DataStructure::ObjZset,
                                                  DataStructure::EncodingSkipList,
                                                  argv[2], argv[3],
                                                  DataStructure::SpareTire,
                                                  DefaultTime);
    if (!flags)
    {
        return Status::IOError("Set error").ToString();
    }
    else
    {
        return Status().ToString();
    }
}
const std::string Server::getCommand(const vector<string> &argv)
{

    std::cout << "getCommand: " << argv[1] << std::endl;
    if (argv.size() != 2)
        return Status::IOError("Wrong parameter").ToString();
    std::string res = database_[dbIndex_]->getKeySpace(DataStructure::ObjString, argv[1]);
    if (res.c_str() == NULL)
    {
        return Status::IOError("Empty Content").ToString();
    }
    else
        return res+"\r\n";
}
const std::string Server::setCommand(const vector<string> &argv)
{
    if (argv.size() != 3)
        return Status::IOError("Wrong parameter").ToString();
    bool flags = database_[dbIndex_]->addKeySpace(DataStructure::ObjString,
                                                  DataStructure::EncodingString,
                                                  argv[1], argv[2],
                                                  DataStructure::SpareTire,
                                                  DefaultTime);
    std::cout << "size: " << database_[dbIndex_]->getKeySpaceStringSize() << std::endl;
    if (!flags)
    {
        return Status::IOError("Set error").ToString();
    }
    else
    {
        return Status().ToString();
    }
}
const std::string Server::bgsaveCommand(const vector<string> &argv)
{
    if (argv.size() != 1)
        return Status::IOError("Wrong parameter").ToString();
    bool flags = CheckStorageConditions();
    if (flags)
        return Status::Ok().ToString();
    
    return Status::IOError("Persistence Error").ToString();
}
const std::string Server::delCommand(const vector<string> &argv)
{
    if (argv.size() != 1)
        return Status::IOError("Wrong parameter").ToString();
    bool flags = database_[dbIndex_]->delKeySpace(DataStructure::ObjString, argv[0]);
    if (flags)
        return Status::Ok().ToString();
    else
        return Status::IOError("Delete Error").ToString();
}
const std::string Server::selectCommand(const vector<string> &argv)
{
    if (argv.size() != 1)
        return Status::IOError("Wrong parameter").ToString();
    int index = atoi(argv[1].c_str());
    database_.push_back(shared_ptr<DataBase>(new DataBase()));
    dbSize_ = database_.size();
    dbIndex_ = index - 1;
    return Status::Ok().ToString();
}
const std::string Server::expireTimeCommand(const vector<string> &argv)
{
    assert(dbIndex_ >= 0);
    if (argv.size() != 2)
        return Status::IOError("Wrong parameter").ToString();
    vector<string> tmp;
    tmp.push_back(argv[1]);
    std::string value = getCommand(tmp);
    if (value == "Empty Reply")
        return value_;
    long long t = atoi(argv[1].c_str());
    bool flags = database_[dbIndex_]->addKeySpace(DataStructure::ObjString,
                                                  DataStructure::EncodingString,
                                                  argv[1], value,
                                                  DataStructure::SpareTire,
                                                  t);
    if (flags)
        return Status::Ok().ToString();
    else
        return Status::IOError("Expire Error").ToString();
}
const std::string Server::hsetCommand(const vector<string> &argv)
{
    assert(dbIndex_ >= 0);
    if (argv.size() != 4)
        return Status::IOError("Wrong parameter").ToString();
    bool flags = database_[dbIndex_]->addKeySpace(DataStructure::ObjHash,
                                                  DataStructure::EncodingString,
                                                  argv[1], argv[2], argv[3],
                                                  DefaultTime);
    if (flags)
        return Status::Ok().ToString();
    else
        return Status::IOError("Expire Error").ToString();
}
const std::string Server::hgetCommand(const vector<string> &argv)
{
    assert(dbIndex_ >= 0);
    if (argv.size() != 2)
        return Status::IOError("Wrong parameter").ToString();
    std::string tmp = database_[dbIndex_]->getKeySpace(DataStructure::ObjHash, argv[1]);
    return tmp;
}
const std::string Server::hgetallCommand(const vector<string> &argv)
{
    assert(dbIndex_ >= 0);
    if (argv.size() != 0)
        return Status::IOError("Wrong parameter").ToString();
    auto it = database_[dbIndex_]->getKeySpaceHashObject().begin();
    std::string tmp, res;
    std::string transition;
    while (it != database_[dbIndex_]->getKeySpaceHashObject().end())
    {
        tmp = database_[dbIndex_]->getKeySpace(DataStructure::ObjHash, it->first);
        transition = it->first + ": " + tmp + " ";
        res += transition;
        it++;
    }
    std::cout << "Server[hgetall]: " << res << std::endl;
    if (res.size() == 0)
        res = Status::NotFound("Not Found hgetall").ToString();
    return res;
}
const std::string Server::rpushCommand(const vector<string> &argv)
{
    assert(dbIndex_ >= 0);
    if (argv.size() < 2)
        return Status::IOError("Wrong parameter").ToString();
    std::string res = std::string();
    std::istringstream str(argv[2]);
    std::string tmp;
    bool flags;
    while (str >> tmp)
    {
        std::cout << "test rpush " << tmp << std::endl;
        flags = database_[dbIndex_]->addKeySpace(DataStructure::ObjList,
                                                 DataStructure::EncodingString,
                                                 argv[1], tmp, DataStructure::SpareTire,
                                                 DefaultTime);
    }
    if (flags)
        res = Status::Ok().ToString();
    else
        res = Status::IOError("Rpush Error").ToString();
    return res;
}
const std::string Server::rpopCommand(const vector<string> &argv)
{
    assert(dbIndex_ >= 0);
    if (argv.size() != 2)
        return Status::IOError("Wrong parameter").ToString();
    std::string res = database_[dbIndex_]->delListObject(argv[1]);
    if (res.size() >= 0)
        return res;
    else
        return Status::IOError("Rpop Error").ToString();
}
std::string Server::commandRequest(Buffer *buf)
{
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
    if (cmd_ == "get")
    {
        vector<string> v;
        v.push_back(cmd_);
        auto it = cmdtable_.find(cmd_);
        if (it == cmdtable_.end())
            res = Status::NotFound("Not Found get").ToString();
        else
        {
            pos = org.find('$', ret);
            ret = org.find(dst.c_str(), pos, 2);
            keylen_ = atoi(org.substr(pos + 1, ret).c_str());
            key_ = org.substr(ret + dst.size(), keylen_);
            v.push_back(key_);
            res = it->second(v);
        }
    }
    else if (cmd_ == "set")
    {
        auto it = cmdtable_.find(cmd_);
        if (it == cmdtable_.end())
            res = Status::NotFound("Not Found set").ToString();
        else
        {
            vector<string> v;
            pos = org.find('$', ret);
            ret = org.find(dst.c_str(), pos, 2);
            keylen_ = atoi(org.substr(pos + 1, ret).c_str());
            key_ = org.substr(ret + dst.size(), keylen_);
            pos = org.find('$', ret);
            ret = org.find(dst.c_str(), pos, 2);
            valuelen_ = atoi(org.substr(pos + 1, ret).c_str());
            value_ = org.substr(ret + dst.size(), valuelen_);
            v.push_back(cmd_);
            v.push_back(key_);
            v.push_back(value_);
            res = it->second(v);
        }
    }
    else if (cmd_ == "bgsave")
    {
        auto it = cmdtable_.find(cmd_);
        vector<string> v;
        v.push_back(cmd_);
        if (it == cmdtable_.end())
            res = Status::NotFound("Not Found This Command bgsave").ToString();
        else
            res = it->second(v);
    }
    else if (cmd_ == "del")
    {
        auto it = cmdtable_.find(cmd_);
        if (it == cmdtable_.end())
            res = Status::NotFound("Not Found This Command del").ToString();
        else
        {
            vector<string> v;
            pos = org.find('$', ret);
            ret = org.find(dst.c_str(), pos, 2);
            keylen_ = atoi(org.substr(pos + 1, ret).c_str());
            key_ = org.substr(ret + dst.size(), keylen_);
            v.push_back(cmd_);
            v.push_back(key_);
            res = it->second(v);
        }
    }
    else if (cmd_ == "select")
    {
        auto it = cmdtable_.find(cmd_);
        if (it == cmdtable_.end())
            res = Status::NotFound("Not Found This Command select").ToString();
        else
        {
            vector<string> v;
            pos = org.find('$', ret);
            ret = org.find(dst.c_str(), pos, 2);
            keylen_ = atoi(org.substr(pos + 1, ret).c_str());
            key_ = org.substr(ret + dst.size(), keylen_);
            v.push_back(cmd_);
            v.push_back(key_);
            res = it->second(v);
        }
    }
    else if (cmd_ == "expire")
    {
        auto it = cmdtable_.find(cmd_);
        if (it == cmdtable_.end())
            res = Status::NotFound("Not Found This Command expire").ToString();
        else
        {
            vector<string> v;
            pos = org.find('$', ret);
            ret = org.find(dst.c_str(), pos, 2);
            keylen_ = atoi(org.substr(pos + 1, ret).c_str());
            key_ = org.substr(ret + dst.size(), keylen_);
            pos = org.find('$', ret);
            ret = org.find(dst.c_str(), pos, 2);
            valuelen_ = atoi(org.substr(pos + 1, ret).c_str());
            value_ = org.substr(ret + dst.size(), valuelen_);
            v.push_back(cmd_);
            v.push_back(key_);
            v.push_back(value_);
            res = it->second(v);
        }
    }
    else if (cmd_ == "hset")
    {
        auto it = cmdtable_.find(cmd_);
        if (it == cmdtable_.end())
            res = Status::NotFound("Not Found This Command hset").ToString();
        else
        {
            vector<string> v;
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
            v.push_back(cmd_);
            v.push_back(key_);
            v.push_back(skey_);
            v.push_back(value_);
            std::cout << "value: " << value_ << "size: " << value_.size() << std::endl;
            res = it->second(v);
        }
    }
    else if (cmd_ == "hget")
    {
        auto it = cmdtable_.find(cmd_);
        if (it == cmdtable_.end())
            res = Status::NotFound("Not Found This Command hget").ToString();
        else
        {
            vector<string> v;
            pos = org.find('$', ret);
            ret = org.find(dst.c_str(), pos, 2);
            keylen_ = atoi(org.substr(pos + 1, ret).c_str());
            key_ = org.substr(ret + dst.size(), keylen_);
            v.push_back(cmd_);
            v.push_back(key_);
            res = it->second(v);
        }
    }
    else if (cmd_ == "rpop")
    {
        auto it = cmdtable_.find(cmd_);
        if (it == cmdtable_.end())
            res = Status::NotFound("Not Found This Command rpop").ToString();
        else
        {
            vector<string> v;
            pos = org.find('$', ret);
            ret = org.find(dst.c_str(), pos, 2);
            keylen_ = atoi(org.substr(pos + 1, ret).c_str());
            key_ = org.substr(ret + dst.size(), keylen_);
            v.push_back(cmd_);
            v.push_back(key_);
            res = it->second(v);
        }
    }
    else if (cmd_ == "rpush")
    {
        auto it = cmdtable_.find(cmd_);
        if (it == cmdtable_.end())
            res = Status::NotFound("Not Found This Command rpush").ToString();
        else
        {
            vector<string> v;
            pos = org.find('$', ret);
            ret = org.find(dst.c_str(), pos, 2);
            keylen_ = atoi(org.substr(pos + 1, ret).c_str());
            key_ = org.substr(ret + dst.size(), keylen_);
            pos = org.find('$', ret);
            ret = org.find(dst.c_str(), pos, 2);
            valuelen_ = atoi(org.substr(pos + 1, ret).c_str());
            value_ = org.substr(ret + dst.size(), valuelen_);
            v.push_back(cmd_);
            v.push_back(key_);
            v.push_back(value_);
            res = it->second(v);
        }
    }
    else if (cmd_ == "hgetall")
    {
        auto it = cmdtable_.find(cmd_);
        vector<string> v;
        v.push_back(cmd_);
        if (it == cmdtable_.end())
            res = Status::NotFound("Not Found This Command hgetall").ToString();
        else
            res = it->second(v);
    } else if(cmd_ == "zadd") {
        auto it = cmdtable_.find(cmd_);
        if (it == cmdtable_.end())
            res = Status::NotFound("Not Found This Command rpush").ToString();
        else
        {
            vector<string> v;
            pos = org.find('$', ret);
            ret = org.find(dst.c_str(), pos, 2);
            keylen_ = atoi(org.substr(pos + 1, ret).c_str());
            key_ = org.substr(ret + dst.size(), keylen_);
            pos = org.find('$', ret);
            ret = org.find(dst.c_str(), pos, 2);
            skeylen_ = atoi(org.substr(pos + 1, ret).c_str());
            skey_ = org.substr(ret + dst.size(), skeylen_);
            pos = org.find('$',ret);
            ret = org.find(dst.c_str(),pos,2);
            valuelen_ = atoi(org.substr(pos + 1, ret).c_str());
            value_ = org.substr(ret+dst.size(),valuelen_);
            v.push_back(cmd_);
            v.push_back(key_);
            v.push_back(skey_);
            v.push_back(value_);
            res = it->second(v);
        }
    }
    else
    {
        std::cout << "[Server::commandRequest:] No Command " << std::endl;
        res = Status::InvalidArgument("No Command").ToString();   
    }
    return res;
}