#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <time.h>

using namespace std;


const int PORT=1997;

int main(){

    srand((unsigned int)time(0));
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("192.168.3.128");



    int fd = socket(AF_INET, SOCK_STREAM, 0);
    
    connect(fd, (sockaddr *)&server_addr, sizeof(server_addr));
    std::string str="GET /images/profile-image.jpg HTTP/1.1\r\n\
Host: 192.168.3.128:1997\r\n\
Connection: keep-alive\r\n\
Cache-Control: max-age=0\r\n\
Upgrade-Insecure-Requests: 1\r\n\
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/104.0.0.0 Safari/537.36\r\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n\
Accept-Encoding: gzip, deflate\r\n\
Accept-Language: zh-CN,zh;q=0.9\r\n\n\n";
    str+='\0';
    int i=0;
    char buf[102400];
    while(1){

        send(fd, str.c_str(), 800, 0);

        int size = recv(fd, buf, 102400, 0);
        cout<<"size: "<<size<<endl;
        std::cout<<buf<<std::endl;
        
        memset(buf, 0, 10240);
        

	    cout<<endl;cout<<endl;cout<<endl;cout<<endl;
        cout<<++i<<endl;
        sleep(5);

    }
    
    return 1;
}
