#include <iostream>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include "threadpool.h"
#include "worker.h"
#include "client_manager.h"
#include <signal.h>

const char *IP="192.168.3.128";
const int PORT=1997;
const int MAX_EVENT=1000;
const int ALARM_TIME=3;//this time is for test;
const int MAX_SLEEP_TIME=9;//this time is for test;

void alarmaciton(int sig){
    client_manager::tik=1;
    //std::cout<<"clocked"<<std::endl;
    alarm(ALARM_TIME);
    return;
}


int main(){

    //server_addr init
    sockaddr_in server_addr;
    sockaddr_in client_addr;
    socklen_t client_addr_len;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    int listen_sock=socket(AF_INET, SOCK_STREAM, 0);

    int reuse = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof( reuse ));
    
    if(bind(listen_sock, (sockaddr *)&server_addr, sizeof(server_addr)) == -1){
        perror("bind");
        exit(-1);
    }

    if(listen(listen_sock,5) == -1){
        perror("listen");
        exit(-1);
    }

    std::cout<<"Server is listening... "<<std::endl;

    //this para is meaningless
    int ep_fd = epoll_create(2);
    if(ep_fd == -1){
        perror("epoll_create");
        exit(-1);
    }

    epoll_event ee[MAX_EVENT];
    epoll_event tmp_ee;

    memset(&tmp_ee, 0, sizeof(tmp_ee));
    tmp_ee.events = EPOLLIN | EPOLLRDHUP;
    tmp_ee.data.fd = listen_sock;

    if(epoll_ctl(ep_fd, EPOLL_CTL_ADD, listen_sock, &tmp_ee) == -1){
        perror("epoll_ADD");
        exit(-1);
    }

    //set SIGPIPE ignored
    struct sigaction sigign;
    sigign.sa_handler = SIG_IGN;
    sigemptyset(&sigign.sa_mask);
    sigign.sa_flags = SA_RESTART;//can stop system call be interupted;
    sigaction(SIGPIPE, &sigign, NULL);

    //set alarm and sigaction
    struct sigaction sigam;
    sigam.sa_handler = alarmaciton;
    sigemptyset(&sigam.sa_mask);
    sigam.sa_flags = SA_RESTART;
    sigaction(SIGALRM, &sigam, NULL);
    alarm(ALARM_TIME);

    client_manager cmm(MAX_EVENT, ep_fd, MAX_SLEEP_TIME);
    worker workthread(&cmm, ep_fd);
    threadpool thread_pool(&workthread);

    while(1){

        int epwt = epoll_wait(ep_fd, ee, MAX_EVENT, -1);

        if(epwt == -1){
            if(errno == EINTR)
                continue;
            else{
                perror("epoll_wait");
                exit(-1);
            }
        }else{
            //process normally
            for(int i = 0; i < epwt; ++i){
                int cursk = ee[i].data.fd;
                if(cursk == listen_sock){
                    //new connect
                    int newsock = accept(cursk, (sockaddr *)&client_addr, &client_addr_len);
                    if(newsock == -1){
                        perror("accept");
                        continue;
                        //exit(-1);
                    }
                    tmp_ee.data.fd = newsock;
                    tmp_ee.events = EPOLLIN | EPOLLRDHUP | EPOLLET | EPOLLONESHOT;
                    if(epoll_ctl(ep_fd, EPOLL_CTL_ADD, newsock, &tmp_ee) == -1){
                        perror("epoll_ADD");
                        exit(-1);
                    }
                    cmm.add(newsock);
                    cmm.setIP(newsock, inet_ntoa(client_addr.sin_addr));
                    printf("client %s is connected\n", inet_ntoa(client_addr.sin_addr));
                }else{
                    if(ee[i].events & EPOLLIN){
                        //read message from kenerl buf 
                        //std::cout<<"reading"<<std::endl;

                        int ret = cmm.read(cursk);
                        if(ret == 0){
                            //link distach
                            std::cout<<cmm.client_info_map[cursk].IP<<" is closed proactively"<<std::endl;
                            cmm.erase(cursk);//fullly close socket and remove all relative data
                        }else{
                            int ret = -1;
                            while(ret == -1)
                                ret = thread_pool.add_work(cursk);
                        }
                        //read accomplished, send to threadpool
                    }else if(ee[i].events & EPOLLOUT){
                        //threadpool finished ,send to client
                        //std::cout<<"writing"<<std::endl;
                        cmm.write(cursk);
                        tmp_ee.data.fd = cursk;
                        tmp_ee.events = EPOLLET | EPOLLIN | EPOLLRDHUP | EPOLLONESHOT;
                        epoll_ctl(ep_fd, EPOLL_CTL_MOD, cursk, &tmp_ee);
                    }

                }
            }
        }


    }






    

    return 1;
}