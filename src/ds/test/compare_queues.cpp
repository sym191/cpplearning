#include <iostream>
#include <thread>
#include <vector>
#include <cassert>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <string>
#include <mutex>
#include <sstream>
#include <memory>
#include "../safequeue.h"
#include "../simple_safequeue.h"
#include "../safequeue_no_uniqueptr.h"

// 用于格式化输出的辅助函数
std::string format_duration(std::chrono::nanoseconds ns) {
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(ns).count();
    std::ostringstream oss;
    oss << ms << "ms";
    return oss.str();
}

// 性能测试模板函数
template<typename QueueType>
void benchmark(QueueType& queue, const std::string& name) {
    constexpr int producer_count = 4;
    constexpr int consumer_count = 4;
    constexpr int items_per_producer = 1000000;
    constexpr int iterations = 1000000;

    std::atomic<int> items_processed(0);
    std::vector<std::thread> threads;
    
    // 用于存储每个线程的时间统计
    std::vector<std::chrono::nanoseconds> producer_times(producer_count);
    std::vector<std::chrono::nanoseconds> consumer_times(consumer_count);
    std::mutex print_mutex; // 用于保护输出

    // 测试多生产者多消费者性能
    std::cout << "=== " << name << " - 多生产者多消费者测试 ===\n";
    auto start = std::chrono::high_resolution_clock::now();

    // 创建生产者线程
    for (int p = 0; p < producer_count; ++p) {
        threads.emplace_back([&queue, p, &producer_times, &print_mutex]() {
            auto thread_start = std::chrono::high_resolution_clock::now();
            
            for (int i = 0; i < items_per_producer; ++i) {
                queue.push(p * items_per_producer + i);
            }
            
            auto thread_end = std::chrono::high_resolution_clock::now();
            producer_times[p] = thread_end - thread_start;
            
            // 输出单个生产者的时间统计
            { 
                std::lock_guard<std::mutex> lock(print_mutex);
                std::cout << "生产者线程 " << p << " 完成: " 
                          << format_duration(producer_times[p]) << " (" 
                          << items_per_producer << " 次push)\n";
            }
        });
    }

    // 创建消费者线程
    for (int c = 0; c < consumer_count; ++c) {
        threads.emplace_back([&queue, c, &items_processed, &consumer_times, &print_mutex]() {
            auto thread_start = std::chrono::high_resolution_clock::now();
            int local_processed = 0;
            
            while (items_processed < producer_count * items_per_producer) {
                int value;
                if (queue.try_pop(value)) {
                    items_processed++;
                    local_processed++;
                } else {
                    std::this_thread::yield();
                }
            }
            
            auto thread_end = std::chrono::high_resolution_clock::now();
            consumer_times[c] = thread_end - thread_start;
            
            // 输出单个消费者的时间统计
            { 
                std::lock_guard<std::mutex> lock(print_mutex);
                std::cout << "消费者线程 " << c << " 完成: " 
                          << format_duration(consumer_times[c]) << " (" 
                          << local_processed << " 次pop)\n";
            }
        });
    }

    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    // 计算平均生产者和消费者时间
    std::chrono::nanoseconds avg_producer_time(0);
    for (const auto& time : producer_times) {
        avg_producer_time += time;
    }
    avg_producer_time /= producer_count;
    
    std::chrono::nanoseconds avg_consumer_time(0);
    for (const auto& time : consumer_times) {
        avg_consumer_time += time;
    }
    avg_consumer_time /= consumer_count;
    
    std::cout << name << " - 多生产者多消费者: " << duration << "ms 处理了 " 
              << items_processed << " 个元素\n";
    std::cout << "平均生产者时间: " << format_duration(avg_producer_time) << "\n";
    std::cout << "平均消费者时间: " << format_duration(avg_consumer_time) << "\n";

    // 清空线程列表
    threads.clear();
    items_processed = 0;

    // 测试交替push/pop性能
    std::cout << "=== " << name << " - 交替push/pop测试 ===\n";
    start = std::chrono::high_resolution_clock::now();
    
    std::chrono::nanoseconds total_push_time(0);
    std::chrono::nanoseconds total_pop_time(0);
    
    for (int i = 0; i < iterations; ++i) {
        auto push_start = std::chrono::high_resolution_clock::now();
        queue.push(i);
        auto push_end = std::chrono::high_resolution_clock::now();
        total_push_time += (push_end - push_start);
        
        int val;
        auto pop_start = std::chrono::high_resolution_clock::now();
        queue.try_pop(val);
        auto pop_end = std::chrono::high_resolution_clock::now();
        total_pop_time += (pop_end - pop_start);
    }

    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    std::cout << name << " - 交替push/pop: " << duration << "ms 完成了 " 
              << iterations << " 次操作\n";
    std::cout << "总Push时间: " << format_duration(total_push_time) << "\n";
    std::cout << "总Pop时间: " << format_duration(total_pop_time) << "\n";
    std::cout << "平均每次Push时间: " << (total_push_time.count() / iterations) << "ns\n";
    std::cout << "平均每次Pop时间: " << (total_pop_time.count() / iterations) << "ns\n";
}

namespace ds {

int test_benchmark() {
    ds::Queue<int> original_queue;
    ds::SimpleQueue<int> simple_queue;
    OptimizedQueue<int> no_unique_queue;

    // 先运行原始队列测试
    benchmark(original_queue, "原始双锁队列(unique_ptr管理内存)");
    std::cout << "\n------------------------\n\n";
    
    // 再运行简单队列测试
    benchmark(simple_queue, "基于std::queue的单锁队列");

    std::cout << "\n-----------------------------\n\n";

    benchmark(no_unique_queue, "增加threadlocal对象池的双锁队列");

    std::cout << "\n性能测试完成！\n";
    return 0;
}

}