#include <iostream>
#include <thread>
#include <vector>
#include <cassert>
#include <chrono>
#include <string>
#include <iomanip>
#include <sstream>
#include "../safequeue_no_uniqueptr.h"

namespace test_queue {



// 用于格式化输出的辅助函数
std::string format_duration(std::chrono::nanoseconds ns) {
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(ns).count();
    std::ostringstream oss;
    oss << ms << "ms";
    return oss.str();
}

// 基本功能测试
void basic_functionality_test() {
    std::cout << "=== 基本功能测试 ===\n";
    
    OptimizedQueue<int> queue;
    
    // 测试空队列
    int val;
    assert(!queue.try_pop(val) && "空队列应该返回false");
    
    // 测试单元素入队出队
    queue.push(42);
    assert(queue.try_pop(val) && val == 42 && "入队元素应该能正确出队");
    assert(!queue.try_pop(val) && "元素弹出后队列应该为空");
    
    // 测试多元素FIFO顺序
    for (int i = 0; i < 10; ++i) {
        queue.push(i);
    }
    
    for (int i = 0; i < 10; ++i) {
        assert(queue.try_pop(val) && val == i && "元素应该按FIFO顺序出队");
    }
    
    assert(!queue.try_pop(val) && "所有元素弹出后队列应该为空");
    
    std::cout << "基本功能测试通过！\n\n";
}

// 自定义类型测试
struct CustomType {
    int id;
    std::string name;
    
    CustomType(int i, std::string n) : id(i), name(std::move(n)) {}
    
    bool operator==(const CustomType& other) const {
        return id == other.id && name == other.name;
    }
};

void custom_type_test() {
    std::cout << "=== 自定义类型测试 ===\n";
    
    OptimizedQueue<CustomType> queue;
    
    // 测试自定义类型入队出队
    queue.push(CustomType(1, "test1"));
    queue.push(CustomType(2, "test2"));
    
    CustomType val(0, "");
    assert(queue.try_pop(val) && val == CustomType(1, "test1") && "自定义类型应该能正确出队");
    assert(queue.try_pop(val) && val == CustomType(2, "test2") && "自定义类型应该保持FIFO顺序");
    
    std::cout << "自定义类型测试通过！\n\n";
}

// 线程安全性测试
void thread_safety_test() {
    std::cout << "=== 线程安全性测试 ===\n";
    
    constexpr int producer_count = 4;
    constexpr int consumer_count = 4;
    constexpr int items_per_producer = 10000;
    
    OptimizedQueue<int> queue;
    std::atomic<int> items_processed(0);
    std::vector<std::thread> threads;
    
    // 创建生产者线程
    for (int p = 0; p < producer_count; ++p) {
        threads.emplace_back([&queue, p]() {
            for (int i = 0; i < items_per_producer; ++i) {
                queue.push(p * items_per_producer + i);
            }
        });
    }
    
    // 创建消费者线程
    for (int c = 0; c < consumer_count; ++c) {
        threads.emplace_back([&queue, &items_processed]() {
            int local_count = 0;
            while (items_processed < producer_count * items_per_producer) {
                int val;
                if (queue.try_pop(val)) {
                    items_processed++;
                    local_count++;
                } else {
                    std::this_thread::yield();
                }
            }
        });
    }
    
    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "线程安全性测试完成：处理了 " << items_processed 
              << " 个元素，预期 " << producer_count * items_per_producer << " 个元素\n";
    assert(items_processed == producer_count * items_per_producer && "所有元素都应该被正确处理");
    
    std::cout << "线程安全性测试通过！\n\n";
}

// 性能测试
void performance_test() {
    std::cout << "=== 性能测试 ===\n";
    
    constexpr int producer_count = 4;
    constexpr int consumer_count = 4;
    constexpr int items_per_producer = 100000;
    constexpr int iterations = 100000;
    
    OptimizedQueue<int> queue;
    std::atomic<int> items_processed(0);
    std::vector<std::thread> threads;
    
    // 测试多生产者多消费者性能
    auto start = std::chrono::high_resolution_clock::now();
    
    // 创建生产者线程
    for (int p = 0; p < producer_count; ++p) {
        threads.emplace_back([&queue, p]() {
            for (int i = 0; i < items_per_producer; ++i) {
                queue.push(p * items_per_producer + i);
            }
        });
    }
    
    // 创建消费者线程
    for (int c = 0; c < consumer_count; ++c) {
        threads.emplace_back([&queue, &items_processed]() {
            while (items_processed < producer_count * items_per_producer) {
                int val;
                if (queue.try_pop(val)) {
                    items_processed++;
                } else {
                    std::this_thread::yield();
                }
            }
        });
    }
    
    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    std::cout << "多生产者多消费者: " << duration << "ms 处理了 " 
              << items_processed << " 个元素\n";
    
    // 清空线程列表
    threads.clear();
    items_processed = 0;
    
    // 测试交替push/pop性能
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        queue.push(i);
        int val;
        queue.try_pop(val);
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    std::cout << "交替push/pop: " << duration << "ms 完成了 " 
              << iterations << " 次操作\n\n";
}

// 大对象测试
struct LargeObj {
    std::vector<char> data;
    
    LargeObj(size_t size) : data(size) {}
};

void large_object_test() {
    std::cout << "=== 大对象测试 ===\n";
    
    constexpr size_t obj_size = 4 * 1024 * 1024; // 4MB
    constexpr int num_objects = 10;
    
    OptimizedQueue<LargeObj> queue;
    
    // 入队大对象
    std::cout << "入队 " << num_objects << " 个大对象（每个 " 
              << obj_size / (1024 * 1024) << "MB）...\n";
    
    for (int i = 0; i < num_objects; ++i) {
        queue.push(LargeObj(obj_size));
    }
    
    // 出队大对象
    std::cout << "出队所有大对象...\n";
    int count = 0;
    LargeObj obj(0);
    while (queue.try_pop(obj)) {
        count++;
    }
    
    std::cout << "成功出队 " << count << " 个大对象\n";
    assert(count == num_objects && "所有大对象都应该被正确处理");
    
    std::cout << "大对象测试通过！\n\n";
}

// 压力测试（长时间运行，验证对象池功能）
void stress_test() {
    std::cout << "=== 压力测试 ===\n";
    
    constexpr int iterations = 1000000; // 100万次操作
    
    OptimizedQueue<int> queue;
    
    std::cout << "执行 " << iterations << " 次push/pop操作以测试对象池...\n";
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        queue.push(i);
        int val;
        queue.try_pop(val);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    std::cout << "压力测试完成: " << duration << "ms 完成了 " 
              << iterations << " 次操作\n\n";
}

}

int OptimizedQueue_test() {
    try {
        test_queue::basic_functionality_test();
        test_queue::custom_type_test();
        test_queue::thread_safety_test();
        test_queue::performance_test();
        test_queue::large_object_test();
        test_queue::stress_test();
        
        std::cout << "所有测试通过！OptimizedQueue 工作正常。\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败：" << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "测试失败：未知异常\n";
        return 1;
    }
}