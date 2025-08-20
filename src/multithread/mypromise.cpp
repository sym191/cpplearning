#include "mypromise.h"
#include <chrono>
#include <future>
#include <memory>
#include <thread>

int _foo1(std::promise<int> p, const int& num){
    p.set_value(num);
    return 0;
}

void mythread::promise::foo(){
    std::promise<int> p;
    auto f = p.get_future();
    auto f2 = std::async(&_foo1, std::move(p), 10);
    std::cout << f.get() << "f2: " << f2.get() << std::endl;
}

// 主线程需要使用某资源，启动线程1加载资源，加载完毕之后通知主线程使用资源

struct A{
    int val_;

    explicit A(int val):val_(val){
        std::cout << "load begin" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));};
};

void load_res(std::promise<std::shared_ptr<A>> p){
    auto a = std::make_shared<A>(100);
    p.set_value(a);
}

void mythread::promise::main_task(){
    std::promise<std::shared_ptr<A>> p;
    auto f = p.get_future();
    auto f2 = std::async(&load_res, std::move(p));
    std::cout << "task begin" << std::endl;
    f2.get();auto ptr = f.get();
    std::cout << "res val: " << ptr->val_ << std::endl;
}