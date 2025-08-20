#include "net/net.h"
#include "ds/ds.h"
#include "util/util.h"
#include <string>
#include "multithread/multithread.h"

int main(){
    // std::array<char, 4> arr = {'1', '2', '3', '4'};
    // auto a = A(arr);
    // net::echo::foo();
    // net::blockreactor::foo(2, 1, 12345, "127.0.0.1");
    // std::cout << "随机数为" << util::random(1,3) << std::endl;
    // auto heap = ds::MinHeap<int>{};
    // for(int i = 0; i < 10000; ++i){
    //     auto num = util::random(0, 100);
    //     heap.push(num);
    // }
    // std::string res = "";
    // while(!heap.empty()){
    //     res += std::to_string(heap.top()) + " ";
    //     heap.pop();
    // }
    // std::cout << res << std::endl;
    // mythread::async::foo();
    mythread::promise::main_task();
}