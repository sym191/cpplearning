#pragma once

#include "random.h"
#include "logger.h"
/*
工具api

// 生成整形/浮点型随机数
T random(const T& begin, const T& end) 

// CRTP单例类模板
// 使用方式： class A : public Singleton<A>{}；
// 通过A::get_instance获取单例A的实例，c++11之后是线程安全的。
class Singleton<T>


// 输出日志到控制台
void log(const std::string& s, Args... args)
void debug(const std::string& s, Args... args)
void warn(const std::string& s, Args... args)
*/