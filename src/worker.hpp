#ifndef WORKER
#define WORKER
#include <string.h>
#include <sys/epoll.h>
#include "client_manager.hpp"


class worker{
public:
    worker(client_manager *_cmm, int _epfd):cmm(_cmm), ep_fd(_epfd){}
    void process(int fd);

private:

    client_manager *cmm;
    int ep_fd;

};

void worker::process(int fd){

    std::cout<<cmm->read_buf[fd]<<std::endl;

    cmm->write_buf[fd] = cmm->read_buf[fd];

    cmm->read_buf[fd] = "";

    epoll_event ee_tmp;
    ee_tmp.data.fd = fd;
    ee_tmp.events = EPOLLOUT | EPOLLET ;
    epoll_ctl(ep_fd, EPOLL_CTL_MOD, fd, &ee_tmp);

}


#endif