#include "../Server/util/Server.cc"

int main (void) {
    Server
     base.addKeySpace(DataStructure::ObjHash,DataStructure::EncodingRaw,"dot","color","white",DefaultTime);
     base.addKeySpace(DataStructure::ObjHash,DataStructure::EncodingRaw,"dot","color","black",DefaultTime);

     base.addKeySpace(DataStructure::ObjHash,DataStructure::EncodingRaw,"dot","name","lala",DefaultTime);
     base.addKeySpace(DataStructure::ObjHash,DataStructure::EncodingRaw,"dot","gender","m",DefaultTime);
    
    // sleep(4);
    // base.rdbLoad();
    std::cout << "get messgae: " << base.getKeySpace(DataStructure::ObjHash,"dot") << std::endl;
    // // base.delKeySpace(DataStructure::ObjString,"name");
    // std::cout << "get messgae: " << base.getKeySpace(DataStructure::ObjList,"gender") << std::endl;
    return 0;
}
