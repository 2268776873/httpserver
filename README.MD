# httpserver
## resource
contains html files, and is copied from https://github.com/gaojingcome/WebServer .
## src
src/ contains server code, main use EPOLL ET mode, client manager  mantain client information like IP and data structure needed by socket like read-write buffer. threadpool manage 10 thread and assign work to them with a task queue, each thread call true worker function in worker.cpp. active timer cooperate with alarm signal and the LRU structure in itself to shut down long sleeping or least recent used socket;
## test
design some client programs to test module function

## problem unsolved
alarm signal can't call a in-class function, and timer is packaged in class LRU, so currently let sigaction function of alarm to change a static in-class variable and use this variable to call clock() when any socket adding or reading.
