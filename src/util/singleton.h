#pragma once

template<typename T>
class Singleton{
protected:
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator= (Singleton&) = delete;
public:
    static T& get_instance(){
        static T instance_;
        return instance_;
    }
};
