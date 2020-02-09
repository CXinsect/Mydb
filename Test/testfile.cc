#include <string.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include "../Server/model.h"
std::string InterceptString(const std::string ptr, int pos1, int pos2) {
    if(pos1 > pos2) std::swap(pos1,pos2);
    int gap = pos2 - pos1;
    std::string ret  = ptr.substr(pos1,gap);
    return ret;
}
int main() {
  // char buf[1024] = {0};
  // std::string tmp = getcwd(buf,sizeof(buf));
  // tmp += "/1.txt";
  // // std::ofstream out (tmp);
  // std::ofstream out;
  // out.open(tmp,std::ios::out | std::ios::app);
  // if(out.is_open()) {
  //     std::string test = "hello,Linux\r\n";
  //     out.write(test.c_str(),test.size());
  // }
  // else std::cout << "No file" << std::endl;
  // printf("%s\n",tmp.c_str());
  // short a = 2;
  // printf("short:%hd\n",a);
  // int ret = sprintf(buf,"%dhello",a);
  // std::cout << "tmp: " << ret << std::endl;
  // out.close();

  // std::list<std::string> v;
  // v.push_back("cx");
  // v.push_back("lala");
  // v.push_back("nini");
  // v.push_back("haha");
  // std::list<std::string>::iterator it = v.begin();
  // char buffer[1024] = {0};
  // char *ptr = buffer;
  // int sum = 0,nwrite = 0;
  // while(it != v.end()) {
  //     nwrite = snprintf(ptr+sum,sizeof(buffer)-sum,"%s",(*it).c_str());
  //     sum += nwrite;
  //     it++;
  // }
  // std::cout << "buffer: "<< buffer << std::endl;
  std::ifstream in;
  char buf[1024] = {0};
  std::string tmp = getcwd(buf, sizeof(buf));
  tmp += "/1.rdb";
  in.open(tmp, std::ios::in);
  if (in.is_open()) {
    while (!in.eof()) {
      memset(buf, 0, sizeof(buf));
      in.read(buf, sizeof(buf));
      // in.getline(buf,sizeof(buf));
    }
  } else {
    std::cout << "open error" << std::endl;
  }
  in.close();
  std::string data = buf;
  std::cout << "data: " << data << std::endl;

  int ret = data.find("DATABASE");
  int pos = data.find("EXPIRETIME");
  std::cout << "string: " << InterceptString(data,ret+8,pos) << std::endl;
  int db_num_ = atoi(InterceptString(data,ret+8,pos).c_str());
  ret = data.find_first_of('^');
  long long expiresTime_ = atoi(InterceptString(data,pos+10,ret).c_str());
  pos = data.find_first_of('!');
  int type_ = atoi(InterceptString(data,ret+1,pos).c_str());
  if (type_ == DataStructure::ObjString) {
    ret = data.find_first_of('@');

    int keyLen_ = atoi(InterceptString(data,pos+1,ret).c_str());
    std::string key_ = data.substr(ret+1,keyLen_);

    pos = data.find('!',ret);
    ret = data.find('$',pos);
    int valueLen_ = atoi(InterceptString(data,pos+1,ret).c_str());
    std::string value_ = data.substr(ret+1,valueLen_);
    pos = data.find('\n',ret);
    data = data.substr(pos+1,data.size());
  }
  if(type_ == DataStructure::ObjHash) {
      std::multimap<std::string, std::string> tmp;
      ret = data.find_first_of('#');
      int mkLen_ = atoi(InterceptString(data,pos+1,ret).c_str());
      std::string key_ =  data.substr(ret+1,mkLen_);
      pos = data.find('!',ret);
      //Skip hash size
      ret = data.find('!',pos+1);
      pos = data.find('@',ret);
      while(ret != -1 && pos != -1) {
        int skeyLen_ = atoi(InterceptString(data,ret+1,pos).c_str());
        std::string skey_ = data.substr(pos+1,skeyLen_);
        ret = data.find('!',pos);
        pos = data.find('$',ret);
        int valueLen_ = atoi(InterceptString(data,ret+1,pos).c_str());
        std::string value_ = data.substr(pos+1,valueLen_);
        tmp.insert(make_pair(skey_,value_));
        ret = data.find('!',pos+1);
        pos = data.find('@',ret);
      }
      std::map<std::pair<std::string ,long long>,std::multimap<std::string, std::string>> Hash_;
      Hash_.insert(make_pair(make_pair(key_,expiresTime_),tmp));  
      pos = data.find('\n',ret);
      data = data.substr(pos+1,data.size()); 
  }
  if(type_ == DataStructure::ObjList) {
      ret = data.find_first_of('@');
      int mkLen_ = atoi(InterceptString(data,pos+1,ret).c_str());
      std::string mkey_ = data.substr(ret+1,mkLen_);
      pos = data.find('!',ret);
      ret = data.find('!',pos+1);
      int ListSize_ = atoi(InterceptString(data,pos+1,ret).c_str());
      std::list <std::string> tmp;
      while(ListSize_-- > 0) {
          pos = data.find('$',ret);
          int valueLen_ = atoi(InterceptString(data,ret+1,pos).c_str());
          std::string value_ = data.substr(pos+1,valueLen_);
          tmp.push_back(value_);
          ret = data.find('!',pos);
      }
      std::map<std::pair<std::string, long long>,std::list<std::string>> List_;
      List_.insert(make_pair(make_pair(mkey_,expiresTime_),tmp));
      pos = data.find('\n',ret);
      data = data.substr(pos+1,data.size());
  }
  std::string test = "DATABASE";
  std::cout << test << std::endl;
  return 0;
}