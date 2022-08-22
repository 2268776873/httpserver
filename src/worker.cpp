#include "worker.h"

#include <iostream>

void worker::process(int fd){

    std::cout<<cmm->read_buf[fd]<<std::endl;

    cmm->write_buf[fd] = cmm->read_buf[fd];

    cmm->read_buf[fd] = "";

    epoll_event ee_tmp;
    ee_tmp.data.fd = fd;
    ee_tmp.events = EPOLLOUT | EPOLLET ;
    epoll_ctl(ep_fd, EPOLL_CTL_MOD, fd, &ee_tmp);

}
