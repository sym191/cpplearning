#include "async.h"
#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include <future>

auto _foo(const int& num){
    std::cout << std::this_thread::get_id() << std::endl; // pid:137946468447936
    return num;
}

void mythread::async::foo(){
    std::cout << std::this_thread::get_id() << std::endl; // pid:137946475603776
    auto future = std::async(std::launch::async, &_foo, 10);
    std::cout << future.get() << std::endl;
}

namespace mythread::task{
    auto _foo(const int& num){
        return num;
    }

    void foo(){
        std::packaged_task<int(const int&)> p(&_foo);
        auto f = p.get_future();
        p(10);
        spdlog::log(spdlog::level::info, "task res: {}", f.get());
    }
}