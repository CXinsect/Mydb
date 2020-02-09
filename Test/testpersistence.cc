// #include "../Server/dataBase.h"
#include "../Server/redisPersistence.cc"

int main (void) {
    std::shared_ptr<DataBase> data(new DataBase(1));
    data->addKeySpace(DataStructure::ObjString,DataStructure::EncodingString,"name","cxdd",DataStructure::SpareTire,11);
    data->addKeySpace(DataStructure::ObjString,DataStructure::EncodingString,"gender","cxdd",DataStructure::SpareTire,11);
//    data->addKeySpace(DataStructure::ObjList,DataStructure::EncodingRaw,"gender","cx",DataStructure::SpareTire,11);
//    data->addKeySpace(DataStructure::ObjList,DataStructure::EncodingRaw,"gender","nini",DataStructure::SpareTire,11);
//    data->addKeySpace(DataStructure::ObjList,DataStructure::EncodingRaw,"gender","haha",DataStructure::SpareTire,11);
//    data->addKeySpace(DataStructure::ObjHash,DataStructure::EncodingRaw,"dot","color","white",11);
//    data->addKeySpace(DataStructure::ObjHash,DataStructure::EncodingRaw,"dot","color","black",11);
//    data->addKeySpace(DataStructure::ObjHash,DataStructure::EncodingRaw,"dot","name","cx",11);
//    data->addKeySpace(DataStructure::ObjHash,DataStructure::EncodingRaw,"dot","gender","m",11);


    
    auto it = data->getKeySpaceStringObject().begin();
    while(it != data->getKeySpaceStringObject().end()) {cout << it->second << endl; it++; }
    Persistence filesave(data,1);
    filesave.CheckStorageConditions();
}