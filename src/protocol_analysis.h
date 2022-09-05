#ifndef PROTOCAL
#define PROTOCAL

#include "client_manager.h"
#include <string>

int protocal_analysis(client_info & cinfo, std::string & s);

int write_reply(client_info & cinfo, std::string &s);

#endif
