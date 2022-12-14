#include "client_manager.h"

#include <iostream>
#include <vector>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include "worker.h"
//#include <sys/epoll.h>
#include <unistd.h>

int client_manager::read(int fd){
    char buf[BUFFER_SIZE];
    int ret = recv(fd, buf, BUFFER_SIZE, 0);
    read_buf[fd] = buf;
    //refresh the LRUtime
    LRU.fresh(fd);
    if(tik)
        clock();//use static member trigger clock
    return ret;
}

void client_manager::clock(){
    tik = 0;//set 0 firstly prevent  simultaneously called by read and add;
    //handler of alarm signal, aim to cut down long sleeping socket
    std::vector<int> sleepr = LRU.sleeper();
    for(int fd : sleepr){
        std::cout<<client_info_map[fd].IP<<" is going to be shutdown for long sleeping"<<std::endl;
        erase(fd);
    }
}

void client_manager::write(int fd){
    char buf[BUFFER_SIZE];
    memcpy(buf, write_buf[fd].data(), write_buf[fd].size()) ;
    //strcpy(buf, write_buf[fd].data());
    //std::cout<<"writed: "<<write_buf[fd]<<std::endl;
    int ret = send(fd, buf, write_buf[fd].size(), 0);
    //occured error with next line code: can't read correctly
    //send(fd, buf, BUFFER_SIZE, 0);
    //std::cout<<"ret: "<<ret<<std::endl;
    // if(ret < write_buf[fd].size()+1){
    //     uint8_t *pic = (uint8_t *)buf + ret;
    //     const char * test = "just a test";
    //     send(fd, test, 24, 0);
    //     //send(fd, pic, write_buf[fd].size() + 1 - ret, 0);
    // }
    std::cout<<"writed: "<<ret<<std::endl;
    write_buf[fd] = "";

}


int client_manager::add(int newfd){
    if(LRU.count(newfd) == 1)
        return -1;
    LRU.add(newfd);
    ++cur_connection;
    if(cur_connection == max_connection){
        //reach the max overload(usually not touch the fd number max 1024 or 65535)
        //when add a new socket, disconnect the LRU one;
        std::cout<<"system is busy,so "<<client_info_map[LRU.last_one()].IP<<" is going to be shutdown for LRU"<<std::endl;
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
    std::cout<<client_info_map[fd].IP<<" is detached"<<std::endl;
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

