#include "../include/util.h"
#include "../include/observer.h"
#include "net/echo.h"
#include "net/blockreactor.h"


int main(){
    // std::array<char, 4> arr = {'1', '2', '3', '4'};
    // auto a = A(arr);
    // net::echo::foo();
    net::blockreactor::foo(2, 1, 12345, "127.0.0.1");
} 