#include "async.h"

auto _foo(const int& num){
    std::cout << std::this_thread::get_id() << std::endl; // pid:137946468447936
    return num;
}

void mythread::async::foo(){
    std::cout << std::this_thread::get_id() << std::endl; // pid:137946475603776
    auto future = std::async(std::launch::async, &_foo, 10);
    std::cout << future.get() << std::endl;
}