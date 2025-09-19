#pragma once

#include <memory>
#include <mutex>

namespace ds{

template<typename T>
class Queue{
private:
    struct Node{
        std::unique_ptr<Node> next_;
        std::unique_ptr<T> data_;
    };
    std::unique_ptr<Node> head_;
    Node* tail_;
    std::mutex head_mutex_;
    std::mutex tail_mutex_;

    Node* get_tail(){
        std::lock_guard<std::mutex> m(tail_mutex_);
        return tail_;
    }

public:
    Queue():head_(new Node()), tail_(head_.get()){}
    Queue(const Queue&) = delete;
    Queue& operator = (const Queue&) = delete;

    std::unique_ptr<T> try_pop(){
        std::lock_guard<std::mutex> m(head_mutex_);
        if(head_.get() == get_tail()) return nullptr;
        auto res = std::move(head_->data_);
        head_ = std::move(head_ -> next_);
        return res;
    }

    bool try_pop(T& val){
        std::lock_guard<std::mutex> m(head_mutex_);
        if(head_.get() == get_tail()) return false;
        val = std::move(*(head_->data_));
        head_ = std::move(head_ -> next_);
        return true;
    }

    void push(T val){
        auto new_node = std::unique_ptr<Node>(new Node());
        auto new_data = std::make_unique<T>(std::move(val));
        auto p_new_node = new_node.get();
        std::lock_guard<std::mutex> m(tail_mutex_);
        tail_ -> next_ = std::move(new_node);
        tail_ -> data_ = std::move(new_data);
        tail_ = p_new_node;
    }
};

int test_queue();

}

