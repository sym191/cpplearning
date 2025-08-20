#pragma once

#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <thread>


namespace net::blockreactor {
    int foo(const unsigned short& domain, const unsigned short& protocol_type, const unsigned short& port, const char* ip);
}