#include "protocol_analysis.h"
#include <unordered_map>
#include <fstream>
#include <sstream>

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
};

std::unordered_map<int, std::string> CODE_STATUS = {
    { 200, "OK" },
    { 400, "Bad Request" },
    { 403, "Forbidden" },
    { 404, "Not Found" },
};

std::unordered_map<int, std::string> CODE_PATH = {
    { 400, "/400.html" },
    { 403, "/403.html" },
    { 404, "/404.html" },
};

int ana_first_line(client_info & cinfo, std::string &s){
    //analysis head;
    return 0;
}

int ans_head_body(client_info & cinfo, std::string &s){

    return 1;
}
int protocal_analysis(client_info &cinfo, std::string &s){

    cinfo.need_t = client_info::needtype::get;
    cinfo.isKeepAlive = 1;
    return 1;

    int end = s.find('\n');
    std::string cur = s.substr(0, end);
    int demand = ana_first_line(cinfo,cur);
    switch (demand){
    case 0:
        cinfo.need_t = client_info::get;
        break;
    case 1:
        cinfo.need_t = client_info::post;
        break;
    default:
        //not allowed
        cinfo.need_t = client_info::wrong;
        break;
    }
    int head = end + 1;
    while(head < s.size()){
        end = s.find('\n', head);
        if(end == -1)
            return -1;
        cur = s.substr(0, end);
        int ret = ans_head_body(cinfo, cur);
        head = end + 1;
    }
    return 1;
}


void addStateLine(client_info &cinfo, std::string &s){
    if(cinfo.need_t == client_info::needtype::get){


    }

    s+="HTTP/1.1 "+ std::to_string(200) + " " + CODE_STATUS[200] + "\r\n";
}

std::string getContentType(){
    return "text/html";
}

void addHeader(client_info &cinfo, std::string &s){
    s += "Connection: ";
    if(cinfo.isKeepAlive) {
        s += "keep-alive\r\n";
        s += "keep-alive: max=6, timeout=120\r\n";
    } else{
        s += "close\r\n";
    }
    s += "Content-type: " + getContentType() + "\r\n";

}

void addContent(client_info &cinfo, std::string &s){
    std::ifstream ifs;
    ifs.open("../resources/index.html");
    std::ostringstream tmp;
    tmp << ifs.rdbuf();
    std::string str = tmp.str();
    s+="Content-length: " + std::to_string(str.size()) + "\r\n\r\n" + str.c_str();
    ifs.close();
}


int write_reply(client_info &cinfo, std::string &s){
    addStateLine(cinfo, s);
    addHeader(cinfo, s);
    addContent(cinfo, s);
    return 0;
}

