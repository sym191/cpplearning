#include "../include/observer.h"

void Subject::add_observer(const std::shared_ptr<Observer>& ptr){
    observer_.emplace_back(ptr);
}

void Subject::remove_observer(const std::shared_ptr<Observer>& ptr){
    int empty = 0;
    for(auto& weakptr : observer_){
        if(weakptr.expired()){
            std::swap(weakptr, observer_.back());
            ++empty;
        }
        else{
            auto sharedptr = weakptr.lock();
            if(sharedptr == ptr){
                std::swap(weakptr, observer_.back());
                ++empty;
                break;
            }
        }
    }
    observer_.resize(observer_.size() - empty);
}

void Subject::notify_observers(){
    auto event = this -> make_event();
    for(auto& observer : observer_){
        if(observer.expired()) continue;
        auto sharedptr = observer.lock();
        sharedptr -> update(event);
    }
}

void WeatherStation::set_temperature(const int& temperature){
    _temperature = temperature;
    this->notify_observers();
}

Event WeatherStation::make_event(){
    return Event(_temperature);
}

void PhoneDisplay::update(const Event& event){
    std::cout <<  _type << ":" << event.get_temperature() << std::endl;
}

void LEDDisplay::update(const Event& event){
    std::cout <<  _type << ":" << event.get_temperature() << std::endl;
}

void test(){
    auto obj = WeatherStation(27);
    auto ptr = std::make_shared<PhoneDisplay>();
    auto ptr2 = std::make_shared<LEDDisplay>();
    obj.add_observer(ptr);
    obj.add_observer(ptr2);
    obj.set_temperature(20);
    obj.remove_observer(ptr);
    obj.set_temperature(30);
}