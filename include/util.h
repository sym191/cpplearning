#pragma once

#include<iostream>
#include <format>
#include <memory>

template<typename... Args>
void println(Args&&... args) {
    std::string res;
    bool first = true;
    ((res += (first ? "" : ", ") + std::format("{}", std::forward<Args>(args)), first = false), ...);
    std::cout << res << std::endl;
}
