#ifndef THREADPOOL
#define THREADPOOL

#include <mutex>
#include <thread>
#include <queue>
#include <semaphore.h>
#include "worker.h"


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



#endif

