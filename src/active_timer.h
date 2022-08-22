#ifndef ACTIVE
#define ACTIVE

#include <list>
#include <vector>
#include <unordered_map>
#include <iterator>


//LRU contains timers which record last reading time and responsible to close connection
class active_timer{
public:
    active_timer(int _fd):fd(_fd),fresh_time(time(0)){}
    void fresh(){
        fresh_time = time(0);
    }
    int fresh_time;
    int fd;
};

class timer_LRU{
public:
    timer_LRU(int _time):max_sleep_time(_time){}
    void add(int fd){
        l.emplace_front(fd);
        m[fd]=l.begin();
    }
    void fresh(int fd){
        m[fd]->fresh();
    }
    void erase(int fd){
        //if connectiong is closed by client, this func is passive
        l.erase(m[fd]);
        m.erase(fd);
    }
    int count(int fd){
        return m.count(fd);
    }
    //return LRU socket
    int last_one(){
        return l.back().fd;
    }

    std::vector<int> sleeper();

private:
    std::list<active_timer> l;
    std::unordered_map<int,std::list<active_timer>::iterator> m;
    int max_sleep_time;

};



#endif