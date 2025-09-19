#pragma once
#include <memory>
#include <mutex>
#include <queue>

namespace ds {

// 基于std::queue的普通线程安全队列实现
template<typename T>
class SimpleQueue {
private:
    std::queue<std::unique_ptr<T>> queue_;
    mutable std::mutex mutex_;

public:
    SimpleQueue() = default;
    SimpleQueue(const SimpleQueue&) = delete;
    SimpleQueue& operator=(const SimpleQueue&) = delete;

    // 返回队头元素的unique_ptr
    std::unique_ptr<T> try_pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return nullptr;
        }
        auto front = std::move(queue_.front());
        queue_.pop();
        return front;
    }

    // 返回队头元素，出参版本
    bool try_pop(T& val) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return false;
        }
        val = std::move(*queue_.front());
        queue_.pop();
        return true;
    }

    // 将元素推入队列
    void push(T val) {
        auto data = std::make_unique<T>(std::move(val));
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(data));
    }
};

int test_benchmark();

} // namespace ds