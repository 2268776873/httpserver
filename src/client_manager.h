#ifndef MES_MAN
#define MES_MAN

#include <unordered_map>
#include "active_timer.h"


const int BUFFER_SIZE = 200000;

struct client_info{
    client_info(){
        http_t = "";
        IP = "";
        target = "";
        pro_t = first;
        need_t = get;
        isKeepAlive = 0;
    }

    std::string http_t;
    std::string IP;
    std::string target;
    enum process{first, head, body, finish} pro_t;
    enum needtype{get, post, wrong = -1} need_t;
    bool isKeepAlive;
    std::unordered_map<std::string, std::string> mes;
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
    int read(int fd);
    void clock();
    void write(int fd);

    std::unordered_map<int, client_info> client_info_map;

    void setIP(int fd, const char * p){
        client_info_map[fd].IP = p;
    }
private:
    client_manager(client_manager &);
    std::unordered_map<int, std::string> read_buf;
    std::unordered_map<int, std::string> write_buf;
    timer_LRU LRU;//if a socket is connected,it must be in manager and in LRU;
    int max_connection;
    int cur_connection;
    int epoll_fd;//when closing a socket because of LRU or timer,need update to epoll in manager;
};


#endif