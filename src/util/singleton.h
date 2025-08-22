#pragma once

template<typename T>
class Singleton{
protected:
    Singleton() = default;
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator= (Singleton&) = delete;
    static T& get_instance(){
        static T instance_;
        return instance_;
    }
};
