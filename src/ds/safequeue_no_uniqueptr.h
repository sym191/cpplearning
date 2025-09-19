#pragma once
#include <mutex>
// #include <memory>
// #include <thread>
#include <vector>
// #include <iostream>

template<typename T>
class OptimizedQueue {
private:
    struct Node {
        Node* next = nullptr; 
        T* data = nullptr;
    };

    Node* head_;
    Node* tail_;
    std::mutex head_mutex_;
    std::mutex tail_mutex_;

    static thread_local std::vector<Node*> local_pool_;

    Node* get_tail() {
        std::lock_guard<std::mutex> lock(tail_mutex_);
        return tail_;
    }

    inline Node* acquire_node() {
        if (!local_pool_.empty()) {
            Node* n = local_pool_.back();
            local_pool_.pop_back();
            n->next = nullptr;
            n->data = nullptr;
            return n;
        }
        return new Node();
    }

    inline void release_node(Node* node) {
        node->next = nullptr;
        delete node->data;
        node->data = nullptr;
        local_pool_.push_back(node);
        if (local_pool_.size() > 1024) {
            for (auto n : local_pool_) delete n;
            local_pool_.clear();
        }
    }

public:
    OptimizedQueue() {
        Node* dummy = new Node();
        head_ = tail_ = dummy;
    }

    ~OptimizedQueue() {
        while (head_) {
            Node* tmp = head_;
            head_ = head_->next;
            delete tmp->data;
            delete tmp;
        }

        for (auto n : local_pool_) {
            delete n->data;
            delete n;
        }
        local_pool_.clear();
    }

    OptimizedQueue(const OptimizedQueue&) = delete;
    OptimizedQueue& operator=(const OptimizedQueue&) = delete;

    void push(T val) {
        Node* node = acquire_node();
        node->data = new T(std::move(val));

        std::lock_guard<std::mutex> lock(tail_mutex_);
        tail_->next = node;
        tail_ = node;
    }

    bool try_pop(T& val) {
        std::lock_guard<std::mutex> lock(head_mutex_);
        Node* first = head_->next;
        if (!first) return false;

        release_node(head_);
        val = std::move(*first->data);
        head_ = first;
        return true;
    }
};

template<typename T>
thread_local std::vector<typename OptimizedQueue<T>::Node*> OptimizedQueue<T>::local_pool_;

int OptimizedQueue_test();