#pragma once
#include <random>
#include <type_traits>

namespace util {
    template <typename T>
    T random(const T& begin, const T& end){
        
        static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>, "只能生成整形和浮点型随机数");
        static thread_local std::random_device rd;
        static thread_local std::mt19937 gen(rd());
        if constexpr(std::is_integral_v<T>){
            std::uniform_int_distribution<> dist(begin, end);
            return dist(gen);
        }
        else if constexpr(std::is_floating_point_v<T>){
            std::uniform_real_distribution<> dist(begin, end);
            return dist(gen);
        }
        return T{};
    }
}
