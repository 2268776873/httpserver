#include "protocol_analysis.h"
#include <unordered_map>
#include <unordered_set>
//#include <fstream>//seem not working well
#include <sstream>
#include <iostream>
#include <regex>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>  
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <fstream>


std::unordered_set<std::string> DEFAULT_HTML = {
    "/index", "/register", "/login",
    "/welcome", "/video", "/picture" };

std::unordered_map<std::string, std::string> SUFFIX_TYPE = {
    { ".html",  "text/html" },
    { ".xml",   "text/xml" },
    { ".xhtml", "application/xhtml+xml" },
    { ".txt",   "text/plain" },
    { ".rtf",   "application/rtf" },
    { ".pdf",   "application/pdf" },
    { ".word",  "application/nsword" },
    { ".png",   "image/png" },
    { ".gif",   "image/gif" },
    { ".jpg",   "image/jpeg" },
    { ".jpeg",  "image/jpeg" },
    { ".au",    "audio/basic" },
    { ".mpeg",  "video/mpeg" },
    { ".mpg",   "video/mpeg" },
    { ".avi",   "video/x-msvideo" },
    { ".gz",    "application/x-gzip" },
    { ".tar",   "application/x-tar" },
    { ".css",   "text/css "},
    { ".js",    "text/javascript "},
    { ".ico",    "image/vnd.microsoft.icon"}
};

std::unordered_map<int, std::string> CODE_STATUS = {
    { 200, "OK" },
    { 400, "Bad Request" },
    { 403, "Forbidden" },
    { 404, "Not Found" },
    { 400, "/400.html" },
    { 403, "/403.html" },
    { 404, "/404.html" },
};

void ans_first_line(client_info & cinfo, std::string &line){
    //analysis head;
    std::regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    std::smatch subMatch;
    if(regex_match(line, subMatch, patten)) {   
        std::string tmp = subMatch[1];
        if(tmp == "GET")
            cinfo.need_t = client_info::get;
        else if(tmp == "POST")
            cinfo.need_t = client_info::post;
        else
            cinfo.need_t = client_info::wrong;
        cinfo.target = subMatch[2];
        cinfo.http_t = subMatch[3];
        cinfo.pro_t = client_info::head;
    }else{
        cinfo.need_t = client_info::wrong;
        return;
    }
    if(cinfo.target == "/"){
        cinfo.target = "/index.html"; 
    }else if(DEFAULT_HTML.count(cinfo.target))
        cinfo.target += ".html";
}

void ans_head(client_info & cinfo, std::string &line){
    std::regex patten("^([^:]*): ?(.*)$");
    std::smatch subMatch;
    //std::cout<<"in ans_head: "<<line<<std::endl;

    if(regex_match(line, subMatch, patten)) {
        cinfo.mes[subMatch[1]] = subMatch[2];
        //std::cout<<subMatch[1]<<std::endl;
        //std::cout<<subMatch[2]<<std::endl;
    }
    else {
        cinfo.pro_t = client_info::body;
    }

}

void ans_body(client_info & cinfo, std::string &line){
    if(cinfo.need_t == client_info::post){

    }else
        cinfo.pro_t = client_info::finish;
}

int protocal_analysis(client_info &cinfo, std::string &s){
    //std::cout<<"full mes: "<<s<<std::endl;
    cinfo.pro_t = client_info::first;
    int end = s.find('\n');
    if(end == -1){
        cinfo.need_t = client_info::wrong;
        return -1;
    }
    std::string cur = s.substr(0, end-1);
    s = s.substr(end+1);
    ans_first_line(cinfo, cur);
    while(cinfo.pro_t != client_info::finish && cinfo.need_t != client_info::wrong && s.size()){
        end = s.find('\n');
        if(end == -1){
            cinfo.need_t = client_info::wrong;
            return -1;
        }
        cur = s.substr(0, end-1);
        //std::cout<<"in while: "<<cur<<std::endl;
        if(cinfo.pro_t == client_info::head)
            ans_head(cinfo, cur);
        else    
            ans_body(cinfo, cur);
        if(end != s.size() - 1)
            s = s.substr(end+1);
        else
            s.clear();
    }
    s.clear();
    if(cinfo.mes["Connection"] == "keep-alive")
        cinfo.isKeepAlive = 1;
    return 0;
}


void addStateLine(client_info &cinfo, std::string &s){
    if(cinfo.need_t == client_info::get){


    }

    s+="HTTP/1.1 "+ std::to_string(200) + " " + CODE_STATUS[200] + "\r\n";
}

void addHeader(client_info &cinfo, std::string &s){
    s += "Connection: ";
    if(cinfo.isKeepAlive) {
        s += "keep-alive\r\n";
        s += "keep-alive: max=6, timeout=120\r\n";
    } else{
        s += "close\r\n";
    }
    int dot = cinfo.target.rfind('.');
    std::string end = cinfo.target.substr(dot);
    s += "Content-type: " + SUFFIX_TYPE[end] + "\r\n";

}

void addContent(client_info &cinfo, std::string &s){
    std::string src = "../resources" + cinfo.target;

	std::ifstream is(src.c_str(), std::ifstream::in);
    if(is.is_open() == 0){
        std::cout<<"file name: "<<src<<" not exist"<<std::endl;
        s = "";
        return ;
    }
    is.seekg(0, is.end);
    int flength = is.tellg();
    is.seekg(0, is.beg);
    char buffer[flength];
    is.read(buffer, flength);
    std::string file(buffer, flength);

    // struct stat filestat;
    // stat(src.c_str(), &filestat);
    // int file = open(src.c_str(), O_RDONLY);
    // if(!file){
    //     std::cout<<"file: "<<src<<"open fail"<<std::endl;
    //     return ;
    // }
    // char* mmp = (char*)mmap(0, filestat.st_size, PROT_READ, MAP_PRIVATE, file, 0);
    // if(*(int *)mmp == -1){
    //     std::cout<<"file: "<<src<<"mmp fail"<<std::endl;
    // }

    std::cout<<"file name: "<<src<<std::endl;
    std::cout<<"file size: "<<flength<<"  is done"<<std::endl;
    s += "Content-length: " + std::to_string(flength) + "\r\n\r\n";
    // if(src=="../resources/images/profile-image.jpg"){
    //     uint8_t *pic = (uint8_t *)mmp;
    //     s += pic;
    // }else
        //s+= mmp;

    s += file;

    // close(file);
    // munmap(mmp, filestat.st_size);
}


int write_reply(client_info &cinfo, std::string &s){
    //std::cout<<"wrting"<<std::endl;

    //for(auto it=cinfo.mes.begin();it!=cinfo.mes.end();++it)
        //std::cout<<it->first<<"\t"<<it->second<<std::endl;
    // if(cinfo.need_t == client_info::wrong){
    //     s = "error";
    //     return 0;
    // }
    addStateLine(cinfo, s);
    addHeader(cinfo, s);
    addContent(cinfo, s);
    return 0;
}

