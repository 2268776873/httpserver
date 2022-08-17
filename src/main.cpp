#include <iostream>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include "threadpool.hpp"
#include "worker.hpp"
#include "client_mes_map.hpp"
#include <signal.h>


const int PORT=1997;
const int MAX_EVENT=10000;
int curlinked = 0;

int main(){

    //server_addr init
    sockaddr_in server_addr;
    sockaddr_in client_addr;
    socklen_t client_addr_len;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

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

    client_mes_map cmm;
    worker workthread(&cmm, ep_fd);
    threadpool thread_pool(&workthread);

    //set SIGPIPE ignored
    struct sigaction saign;
    saign.sa_handler = SIG_IGN;
    sigemptyset(&saign.sa_mask);
    saign.sa_flags = 0;
    sigaction(SIGPIPE, &saign, NULL);


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
                        exit(-1);
                    }
                    tmp_ee.data.fd = newsock;
                    tmp_ee.events = EPOLLIN | EPOLLRDHUP | EPOLLET | EPOLLONESHOT;
                    if(epoll_ctl(ep_fd, EPOLL_CTL_ADD, newsock, &tmp_ee) == -1){
                        perror("epoll_ADD");
                        exit(-1);
                    }

                    printf("client %s is connected\n", inet_ntoa(client_addr.sin_addr));
                    curlinked++;

                    //linked over half
                    if(curlinked == MAX_EVENT/2){
                        
                    }
                }else{
                    if(ee[i].events & EPOLLIN){
                        //read message from kenerl buf 
                        //std::cout<<"reading"<<std::endl;

                        cmm.add(cursk);
                        int ret = cmm.read(cursk);
                        if(ret == 0){
                            //link distach
                            std::cout<<"a link is detached"<<std::endl;
                            cmm.erase(cursk);
                            epoll_ctl(ep_fd, EPOLL_CTL_DEL, cursk, NULL);
                            close(cursk);
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