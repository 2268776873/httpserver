#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <time.h>




const int PORT=1997;

int main(){

    srand((unsigned int)time(0));
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("192.168.3.128");



    int fd = socket(AF_INET, SOCK_STREAM, 0);
    
    connect(fd, (sockaddr *)&server_addr, sizeof(server_addr));
    int i=0;
    char buf[7000];

    while(1){
        std::string str=std::to_string(i);
        str+=" times communication from ";
        str+=std::to_string(getpid());
        str+='\0';
        std::cout<<"sended: "<<str<<std::endl;
        send(fd, str.c_str(), 300, 0);

        memset(buf, 0, 1024);
        recv(fd, buf, 1024, 0);
        std::cout<<"revced: "<<buf<<std::endl;

        ++i;
        usleep(300000);

    }
    
    return 1;
}