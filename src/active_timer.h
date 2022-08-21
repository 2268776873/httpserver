#ifndef ACTIVE
#define ACTIVE

#include <sys/time.h>
#include <time.h>
#include <list>
#include <unordered_map>
#include <iterator>

class active_timer{
public:
    active_timer(int _fd):fd(_fd),fresh_time(time(0)){}
    void fresh(){
        fresh_time = time(0);
    }
    void destory(){

    }
private:
    int fresh_time;
    int fd;
};

class timer_LRU{
public:
    void add(int fd){
        active_timer *timerp = new active_timer(fd);
        l.emplace_front(timerp);
        m[fd]=l.begin();
    }
    void fresh(int fd){

    }
private:
    std::list<active_timer*> l;
    std::unordered_map<int,std::list<active_timer*>::iterator> m;

};


void clear_dead_connection(){

}



#endif