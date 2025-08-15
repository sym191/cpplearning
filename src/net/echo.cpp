#include "echo.h"
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <vector>
#include <unistd.h>

void net::foo(){
    int sc = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in ad{AF_INET, htons(12345), inet_addr("127.0.0.1")};

    struct sockaddr* addr = reinterpret_cast<sockaddr*>(&ad);
    if (bind(sc, addr, sizeof(*addr)) < 0){
        std::cout << "bind err" << std::endl;
        return;
    }
    if(listen(sc, 10) < 0){
        std::cout << "listen err" << std::endl;
        return;
    }
    socklen_t len = sizeof(*addr);
    int consc = accept(sc, addr, &len);
    if(consc < 0){
        std::cout << "accept err" << std::endl;
    }
    std::vector<char> buffer;
    buffer.reserve(1000);
    while(true){
        auto n = read(consc, buffer.data(), 11);
        std::cout << "get string size:" << n << std::endl;
        if(n <= 0){
            continue;
        }
        char* str = new char[n];
        std::memcpy(str, buffer.data(), n);
        std::cout << str << std::endl;
    }
}