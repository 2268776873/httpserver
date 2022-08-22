#ifndef WORKER
#define WORKER

#include <string.h>
#include <sys/epoll.h>
#include "client_manager.h"

class worker{
public:
    worker(client_manager *_cmm, int _epfd):cmm(_cmm), ep_fd(_epfd){}
    void process(int fd);

private:

    client_manager *cmm;
    int ep_fd;

};


#endif