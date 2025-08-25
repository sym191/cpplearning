#pragma once
#include "spdlog/spdlog.h"


template<typename ...Args>
inline void log(fmt::format_string<Args...> s, Args&&... args){
    spdlog::info(s, std::forward<Args>(args)...);
}

template <typename... Args>
inline void debug(fmt::format_string<Args...> s, Args &&...args) {
    spdlog::debug(s, std::forward<Args>(args)...);
}

template <typename... Args>
inline void warn(fmt::format_string<Args...> s, Args &&...args) {
    spdlog::warn(s, std::forward<Args>(args)...);
}