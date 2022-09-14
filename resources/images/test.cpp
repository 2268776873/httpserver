#include <sys/mman.h>
#include <sys/stat.h>  
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <fstream>

using namespace std;

int main(){

	std::string src = "instagram-image2.jpg";
	
	
	std::ifstream is(src.c_str(), std::ifstream::in);
    is.seekg(0, is.end);
    int flength = is.tellg();
    is.seekg(0, is.beg);
    char * buffer = new char[flength];
    is.read(buffer, flength);
    std::string image(buffer, flength);
	cout<<buffer<<endl;
	
	
	return 0;
	
   	struct stat filestat;
    	stat(src.c_str(), &filestat);
    int file = open(src.c_str(), O_RDONLY);
    FILE* f=fopen(src.c_str(),"rb");
    std::cout<<"file name: "<<src<<std::endl;
    char tmp[filestat.st_size+1];
    
    
    fread(tmp,filestat.st_size+1,1,f);
    std::cout<<"fread: \n"<<tmp<<std::endl;

    
    uint8_t * mmp = (uint8_t *)mmap(0, filestat.st_size+1, PROT_READ, MAP_PRIVATE, file, 0);
    //if(src=="../resources/images/profile-image.jpg")
	std::cout<<"mmp: \n"<<mmp<<std::endl;
	printf("%s\n",mmp);
    if(*(int *)mmp == -1){
	std::cout<<"file: "<<src<<"mmp fail"<<std::endl;
    }
    read(file, tmp, filestat.st_size+1);
    std::cout<<std::endl;
    
    std::cout<<"tmp: \n"<<tmp<<std::endl;
std::cout<<std::endl;
    std::cout<<"file size: "<<filestat.st_size<<std::endl;
    //s+="Content-length: " + std::to_string(filestat.st_size) + "\r\n\r\n" + tmp;
    close(file);
    munmap(mmp, filestat.st_size);

	return 0;
}
