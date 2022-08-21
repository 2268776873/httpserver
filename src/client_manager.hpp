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

struct client_info{
    enum httptype{http1, http1_1, http2_0} http_t;

};
//manage all socket except listen socket
class client_manager{ 
public:
friend class worker;

    static int tik;

    client_manager(int _max_connection, int ep_fd, int _time):max_connection(_max_connection),\
    epoll_fd(ep_fd),cur_connection(1),LRU(_time){}
    ~client_manager(){}

    int add(int newfd);
    int erase(int fd);

    int read(int fd){
        char buf[BUFFER_SIZE];
        int ret = recv(fd, buf, BUFFER_SIZE, 0);
        read_buf[fd] = buf;
        //refresh the LRUtime
        LRU.fresh(fd);
        if(tik)
            clock();//use static member trigger clock
        return ret;
    }

    void clock(){
        //handler of alarm signal, aim to cut down long sleeping socket
        std::vector<int> sleepr = LRU.sleeper();
        for(int fd : sleepr){
            erase(fd);
        }
        tik = 0;
    }

    void write(int fd){
        char buf[BUFFER_SIZE];
        strcpy(buf, write_buf[fd].c_str()) ;
        send(fd, buf, sizeof(buf), 0);
    }


private:
    client_manager(client_manager &);
    std::unordered_map<int, std::string> read_buf;
    std::unordered_map<int, std::string> write_buf;
    std::unordered_map<int, client_info> client_info_map;
    timer_LRU LRU;//if a socket is connected,it must be in manager and in LRU;
    int max_connection;
    int cur_connection;
    int epoll_fd;//when closing a socket because of LRU or timer,need update to epoll in manager;
};



int client_manager::add(int newfd){
    if(LRU.count(newfd) == 1)
        return -1;
    LRU.add(newfd);
    ++cur_connection;
    if(cur_connection == max_connection){
        //reach the max overload(usually not touch the fd number max 1024 or 65535)
        //when add a new socket, disconnect the LRU one;
        erase(LRU.last_one());
        //std::cout<<"no action"<<std::endl; 
    }
    if(tik)
        clock();//use static member trigger clock
    return 1;
}

int client_manager::erase(int fd){
    if(LRU.count(fd) == 0)
        return -1;
    read_buf.erase(fd);
    write_buf.erase(fd);
    client_info_map.erase(fd);
    LRU.erase(fd);
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    --cur_connection;
    return 1;
}

int client_manager::tik = 0;

#endif