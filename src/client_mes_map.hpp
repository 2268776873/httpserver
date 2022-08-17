#ifndef MES_MAP
#define MES_MAP
#include <unordered_map>
#include <vector>
#include <arpa/inet.h>



const int BUFFER_SIZE = 1024;
struct buffer{
    char wrbuf[BUFFER_SIZE];
    char rdbuf[BUFFER_SIZE];
};


struct client_browser{
    enum httptype{http1, http1_1, http2_0} http_t;

};

class client_mes_map{ 
public:

    client_mes_map(){}
    ~client_mes_map();

    int add(int newfd);
    int erase(int fd);

    int read(int fd){
        int ret = recv(fd, buffer_map[fd]->rdbuf, BUFFER_SIZE, 0);
        return ret;
    }



    int write(int fd){
        return send(fd, buffer_map[fd]->wrbuf, sizeof(buffer_map[fd]->wrbuf), 0);
    }

    buffer * get_buffer(int fd){
        if(buffer_map.count(fd) == 0)
            return nullptr;
        return buffer_map[fd];
    }

    client_browser * get_brower_info(int fd){
        if(client_browser_map.count(fd) == 0)
            return nullptr;
        return client_browser_map[fd];
    }

private:
    client_mes_map(client_mes_map &);
    std::unordered_map<int, buffer*> buffer_map;
    std::unordered_map<int, client_browser*> client_browser_map;
};

client_mes_map::~client_mes_map(){
    for(auto [_,buf]:buffer_map)
        delete buf;
    for(auto [_,cb]:client_browser_map)
        delete cb;
}

int client_mes_map::add(int newfd){
    if(buffer_map.count(newfd) == 1)
        return -1;
    buffer_map[newfd] = new buffer;
    client_browser_map[newfd] = new client_browser;
    return 1;
}

int client_mes_map::erase(int fd){
    if(buffer_map.count(fd) == 0)
        return -1;
    delete buffer_map[fd];
    delete client_browser_map[fd];
    buffer_map.erase(fd);
    client_browser_map.erase(fd);
    return 1;
}




#endif