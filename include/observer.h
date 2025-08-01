#pragma once

#include "util.h"
#include <vector>
#include <memory>


struct Event{
    Event(const int& temperature):_temperature(temperature){}
    const int get_temperature() const{
        return _temperature;
    }
private:
    int _temperature;
};

class Observer{
public:
    virtual void update(const Event& event) = 0;
    virtual ~Observer() = default;
private:

};

class PhoneDisplay : public Observer{
public:
    void update(const Event& event) override;
private:
    std::string _type = "phone";
};

class LEDDisplay : public Observer{
public:
    void update(const Event& event) override;

private:
    std::string _type = "led";
};

class Subject{
public:
    Subject():observer_{}{}
    virtual ~Subject(){}
    virtual void notify_observers();
    virtual Event make_event() = 0;
    void add_observer(const std::shared_ptr<Observer>& ptr);
    void remove_observer(const std::shared_ptr<Observer>& ptr);
protected:
    std::vector<std::weak_ptr<Observer>> observer_;
    // std::unordered_set<std::weak_ptr<Observer>> observer;
};

class WeatherStation : public Subject{
public:
    WeatherStation(const int& temperature): _temperature(temperature), Subject(){}
    void set_temperature(const int& temperature);
    Event make_event();
private:
    int _temperature;
};

void test();