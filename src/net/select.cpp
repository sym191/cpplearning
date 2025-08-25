// #ifdef __linux__

// #include "select.h"
// #include <cstddef>
// #include <cstring>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <thread>

// void _block_read(int sc){
//     char buffer[1024] = {};
//     while(true){
//         size_t len = read(sc, buffer, 11);
//         if(len < 0){
//             std::cout << "socket" << sc << "read err" << std::endl;
//             continue;
//         }
//         if(len == 0){
//             std::cout << "socket" << sc << " end connect" << std::endl;
//             return;
//         }
//         char* msg = new char[len+1];
//         msg[len] = '\0';
//         memcpy(msg, buffer, len);
//         memset(buffer, 0, len);
//         std::cout << "socket" << sc << " threadid:" << std::this_thread::get_id() << " read msg:" << len << ":" << msg << std::endl;
//         delete[] msg;
//     }
//     close(sc);
// }

// int net::select::tcp_test(const unsigned short& port,const char* ip){
//     int sc = socket(AF_INET, SOCK_STREAM, 0);
//     struct sockaddr_in ad{AF_INET, htons(port), inet_addr(ip)};
//     auto sockaddr = reinterpret_cast<struct sockaddr*>(&ad);
//     if(bind(sc, sockaddr, sizeof(*sockaddr)) < 0){
//         std::cout << "bind err" << sc << std::endl;
//         return -1; // bind err
//     }
//     if(listen(sc, 100) < 0){
//         std::cout << "listen err" << sc << std::endl;
//         return -2; // listen err
//     }
//     socklen_t len = sizeof(ad);
//     std::vector<std::thread> t;
//     std::vector<int> socs;
//     while(true){
//         struct sockaddr_in client_addr {};
//         socklen_t client_len = sizeof(client_addr);
//         auto con_soc = accept(sc, reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);
//         if(con_soc < 0){
//             std::cout << "connect err" << std::endl;
//             continue;
//         }
//         std::cout << con_soc << "connect success!" << "client ip:" << inet_ntoa(client_addr.sin_addr) << "\tclient port:" << ntohs(client_addr.sin_port) << std::endl;
//         // t.emplace_back(&_block_read, con_soc);
//         socs.push_back(con_soc);
//     }
//     // int res = select(sc, );
//     for(auto& _t : t){
//         _t.join();
//     }
//     return 0;
// }



// #endif