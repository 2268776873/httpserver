#ifndef WORKER
#define WORKER
#include <string.h>
#include <sys/epoll.h>
#include "client_mes_map.hpp"

class worker{
public:
    worker(client_mes_map *_cmm, int _epfd):cmm(_cmm), ep_fd(_epfd){}
    void process(int fd);

private:

    client_mes_map *cmm;
    int ep_fd;

};

void worker::process(int fd){

    buffer * buf = cmm->get_buffer(fd);
    std::cout<<buf->rdbuf<<std::endl;

    strcpy(buf->wrbuf, buf->rdbuf);
    memset(buf->rdbuf, 0, BUFFER_SIZE);


    epoll_event ee_tmp;
    ee_tmp.data.fd = fd;
    ee_tmp.events = EPOLLOUT | EPOLLET ;
    epoll_ctl(ep_fd, EPOLL_CTL_MOD, fd, &ee_tmp);

}


#endif