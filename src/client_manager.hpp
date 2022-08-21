#ifndef MES_MAN
#define MES_MAN
#include <unordered_map>
#include <vector>
#include <arpa/inet.h>
#include "active_timer.h"
#include <string>
#include <string.h>
#include "worker.hpp"

const int BUFFER_SIZE = 1024;

struct client_browser{
    enum httptype{http1, http1_1, http2_0} http_t;

};

class client_manager{ 
public:
friend class worker;
    client_manager(){}
    ~client_manager();

    int add(int newfd);
    int erase(int fd);

    int read(int fd){
        char buf[BUFFER_SIZE];
        int ret = recv(fd, buf, BUFFER_SIZE, 0);
        read_buf[fd] = buf;
        return ret;
    }



    void write(int fd){
        char buf[BUFFER_SIZE];
        strcpy(buf, write_buf[fd].c_str()) ;
        send(fd, buf, sizeof(buf), 0);
    }

    client_browser * get_brower_info(int fd){
        if(client_browser_map.count(fd) == 0)
            return nullptr;
        return client_browser_map[fd];
    }

private:
    client_manager(client_manager &);
    std::unordered_map<int, std::string> read_buf;
    std::unordered_map<int, std::string> write_buf;
    std::unordered_map<int, client_browser*> client_browser_map;
};

client_manager::~client_manager(){
    for(auto [_,cb]:client_browser_map)
        delete cb;
}

int client_manager::add(int newfd){
    if(client_browser_map.count(newfd) == 1)
        return -1;
    client_browser_map[newfd] = new client_browser;
    return 1;
}

int client_manager::erase(int fd){
    if(client_browser_map.count(fd) == 0)
        return -1;
    delete client_browser_map[fd];
    read_buf.erase(fd);
    write_buf.erase(fd);
    client_browser_map.erase(fd);
    return 1;
}




#endif