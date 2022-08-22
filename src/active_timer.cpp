#include "active_timer.h"

#include <iostream>
#include <sys/time.h>
#include <time.h>

std::vector<int> timer_LRU::sleeper(){
    int cur=time(0);
    std::vector<int> sleep;
    for(active_timer time : l){
        if(cur - time.fresh_time > max_sleep_time){
            sleep.push_back(time.fd);
        }
    }
    return sleep;
}