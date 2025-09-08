
#ifdef __linux__

#include "select.h"
#include <cstddef>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <bits/types/struct_timeval.h>
#include <mutex>
#include <sys/select.h>
#include <unistd.h>
#include <unordered_set>

namespace net::select {

void _block_read(int sc){
    char buffer[1024] = {};
    while(true){
        size_t len = read(sc, buffer, 11);
        if(len < 0){
            std::cout << "socket" << sc << "read err" << std::endl;
            continue;
        }
        if(len == 0){
            std::cout << "socket" << sc << " end connect" << std::endl;
            return;
        }
        char* msg = new char[len+1];
        msg[len] = '\0';
        memcpy(msg, buffer, len);
        memset(buffer, 0, len);
        std::cout << "socket" << sc << " threadid:" << std::this_thread::get_id() << " read msg:" << len << ":" << msg << std::endl;
        delete[] msg;
    }
    close(sc);
}

struct SocketManager{
private:
    std::unordered_set<int> sockets_;
    std::mutex mutex_;
public:
    SocketManager():sockets_(), mutex_(){}

    void add_socket(const int& socket){
        std::lock_guard<std::mutex> guard(mutex_);
        sockets_.insert(socket);
        log("add socket, fd: {}", socket);
    }

    void remove_socket(const int& socket){
        {
            std::lock_guard<std::mutex> guard(mutex_);
            auto pos = sockets_.find(socket);
            if(pos != sockets_.end()){
                sockets_.erase(pos);
            }
        }
        log("close socket, fd: {}", socket);
        close(socket);
    }

    std::unordered_set<int> get_socket(){
        std::lock_guard<std::mutex> guard(mutex_);
        return sockets_;
    }
};

void keep_accept(SocketManager& mgr, const int& listen_socket){
    while(true){
        struct sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        auto client_socket = accept(listen_socket, reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);
        if(client_socket < 0){
            log("accept err");
            continue;
        }
        log("accept successful! client port:{}, ip:{}", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        mgr.add_socket(client_socket);
    }
}

void try_select(SocketManager& mgr){
    fd_set rfds, wfds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    auto socket_set = mgr.get_socket();
    int max_socket = 0;
    for(const auto& socket : socket_set){
        max_socket = (socket > max_socket ? socket : max_socket);
        FD_SET(socket, &rfds);
    }
    struct timeval tv{5, 0};
    int n = ::select(max_socket + 1, &rfds, &wfds, nullptr, &tv);
    if(n < 0){
        log("select false");
        return;
    }
    auto new_socket_set = mgr.get_socket();
    for(const auto& socket : socket_set){
        if(socket > 0 && FD_ISSET(socket, &rfds)){
            char buf[4096];
            ssize_t m = read(socket, buf, sizeof(buf));
            if(m > 0){
                log("fd:{}, msg:{}", socket, buf);
            }
            else{
                mgr.remove_socket(socket);
            }
        }
    }
}

int tcp_test(const unsigned short& port,const char* ip){
    int sc = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in ad{AF_INET, htons(port), inet_addr(ip)};
    auto sockaddr = reinterpret_cast<struct sockaddr*>(&ad);
    if(bind(sc, sockaddr, sizeof(*sockaddr)) < 0){
        std::cout << "bind err" << sc << std::endl;
        return -1; // bind err
    }
    if(listen(sc, 100) < 0){
        std::cout << "listen err" << sc << std::endl;
        return -2; // listen err
    }
    SocketManager mgr{};
    std::thread t([&](){
        keep_accept(mgr, sc);
    });
    while(true){
        try_select(mgr);
    }
    t.join();
    return 0;
}

}

#endif