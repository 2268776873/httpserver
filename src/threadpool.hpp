#ifndef THREADPOOL
#define THREADPOOL

#include <thread>
#include <queue>
#include <mutex>
#include <semaphore.h>
#include <iostream>
#include <errno.h>
#include "worker.hpp"
//#include <cstdio>
#include <exception>

const int MAX_THREADNUM=10;
const int MAX_QUEUE_SIZE=10000;



class threadpool{

public:
    threadpool(worker *_pworker);
    ~threadpool();
    static void thread_api(threadpool *);
    void thread_run_loop();
    int add_work(int);

private:
    std::mutex queue_mutex;
    sem_t queue_space;
    sem_t needed_work;
    int  over;//if thraedpool is closing ,turn 1;
    std::queue<int> work_quque;
    std::thread* thread_pool[MAX_THREADNUM];

    worker *pworker;
};

threadpool::threadpool(worker *_pworker) : pworker(_pworker), over(0){
    for(int i=0;i<MAX_THREADNUM;++i){
        thread_pool[i] = new std::thread (thread_api, this);
        std::cout<<"thread "<<i<<" is established"<<std::endl;
        thread_pool[i]->detach();
    }
    sem_init(&queue_space, 0, MAX_QUEUE_SIZE);
    sem_init(&needed_work, 0, 0);

}

threadpool::~threadpool(){
    over = 1;
    sem_destroy(&queue_space);
    sem_destroy(&needed_work);
    for(int i=0;i<MAX_THREADNUM;++i)
        delete thread_pool[i];
    
}

void threadpool::thread_run_loop(){
    while(!over){
        if(sem_wait(&needed_work) == -1){
            if(errno == EINTR)
                continue;
            else{
                std::cout<<std::this_thread::get_id()<<" thread occured a problem, and exited"<<std::endl;
                throw std::exception();
            }
        }
        int fd_to_work = -1;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            fd_to_work = work_quque.front();
            work_quque.pop();
        }
        if(fd_to_work == -1)
            continue;
        pworker->process(fd_to_work);
        sem_post(&queue_space);
    }
}

void threadpool::thread_api(threadpool *pool){
    pool->thread_run_loop();
}

int threadpool::add_work(int fd){
    if(sem_wait(&queue_space) == -1){
        if(errno == EINTR)
            return -1;
        else{
            std::cout<<"add work occured a problem"<<std::endl;
            throw std::exception();
        }
    }
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        work_quque.push(fd);
        sem_post(&needed_work);
    }
    return 1;
}



#endif

