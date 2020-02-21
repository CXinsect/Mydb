#include "../Client/Client.h"
#include <ctime>
int main (int argc,char* argv[]) {
    Client client(6379,"127.0.0.1",600);
    client.Connect();
    client.sendRequest("set name test");
    client.sendRequest("get name");
    client.sendRequest("hset tree color black");
    client.sendRequest("hset tree height high");
    client.sendRequest("hget tree");
    client.sendRequest("rpush list didi jiejie");
    client.sendRequest("rpop list");
    client.sendRequest("zadd zset 1 apple 2 cherry");
    client.sendRequest("zcount zset 0 3");
}