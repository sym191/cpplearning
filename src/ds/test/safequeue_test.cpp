#include "../safequeue.h"
#include <iostream>
#include <thread>
#include <vector>
#include <cassert>
#include <chrono>

// 基本功能测试
void basic_test() {
    std::cout << "=== 基本功能测试 ===\n";
    ds::Queue<int> q;
    
    // 测试空队列弹出
    auto val = q.try_pop();
    assert(val == nullptr);
    
    int out_val;
    bool result = q.try_pop(out_val);
    assert(result == false);
    
    // 测试入队出队
    q.push(10);
    q.push(20);
    q.push(30);
    
    // 使用第一种出队方式
    auto val1 = q.try_pop();
    assert(*val1 == 10);
    
    // 使用第二种出队方式
    result = q.try_pop(out_val);
    assert(result == true);
    assert(out_val == 20);
    
    result = q.try_pop(out_val);
    assert(result == true);
    assert(out_val == 30);
    
    // 队列再次为空
    result = q.try_pop(out_val);
    assert(result == false);
    
    std::cout << "基本功能测试通过！\n";
}

// 测试自定义类型
void custom_type_test() {
    std::cout << "=== 自定义类型测试 ===\n";
    
    struct TestObj {
        int id;
        std::string name;
        
        TestObj(int i, std::string n) : id(i), name(std::move(n)) {}
        
        bool operator==(const TestObj& other) const {
            return id == other.id && name == other.name;
        }
    };
    
    ds::Queue<TestObj> q;
    q.push(TestObj(1, "obj1"));
    q.push(TestObj(2, "obj2"));
    
    auto obj1 = q.try_pop();
    assert(obj1->id == 1);
    assert(obj1->name == "obj1");
    
    TestObj obj2(0, "");
    bool result = q.try_pop(obj2);
    assert(result == true);
    assert(obj2.id == 2);
    assert(obj2.name == "obj2");
    
    std::cout << "自定义类型测试通过！\n";
}

// 线程安全性测试 - 多生产者多消费者
void thread_safety_test() {
    std::cout << "=== 线程安全性测试 ===\n";
    
    ds::Queue<int> q;
    constexpr int producer_count = 4;
    constexpr int consumer_count = 4;
    constexpr int items_per_producer = 1000;
    
    std::atomic<int> items_processed(0);
    std::vector<std::thread> threads;
    
    // 创建生产者线程
    for (int p = 0; p < producer_count; ++p) {
        threads.emplace_back([&q, p]() {
            for (int i = 0; i < items_per_producer; ++i) {
                int value = p * items_per_producer + i;
                q.push(value);
                // 随机小延迟，增加并发冲突的可能性
                if (i % 10 == 0) {
                    std::this_thread::sleep_for(std::chrono::microseconds(1));
                }
            }
        });
    }
    
    // 创建消费者线程
    for (int c = 0; c < consumer_count; ++c) {
        threads.emplace_back([&q, &items_processed]() {
            int local_count = 0;
            while (items_processed < producer_count * items_per_producer) {
                int value;
                if (q.try_pop(value)) {
                    local_count++;
                    items_processed++;
                } else {
                    // 队列为空时短暂让出CPU
                    std::this_thread::yield();
                }
            }
            std::cout << "消费者处理了 " << local_count << " 个元素\n";
        });
    }
    
    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }
    
    assert(items_processed == producer_count * items_per_producer);
    std::cout << "线程安全性测试通过！总共处理了 " << items_processed << " 个元素\n";
}

// 测试边界条件 - 大对象测试
void large_object_test() {
    std::cout << "=== 大对象测试 ===\n";
    
    struct LargeObj {
        std::vector<int> data;
        
        explicit LargeObj(size_t size) : data(size, 42) {}
    };
    
    ds::Queue<LargeObj> q;
    
    // 创建和推送大对象
    q.push(LargeObj(1000000)); // 约4MB的数据
    
    // 弹出并验证
    auto large_obj = q.try_pop();
    assert(large_obj != nullptr);
    assert(large_obj->data.size() == 1000000);
    assert(large_obj->data[0] == 42);
    
    std::cout << "大对象测试通过！\n";
}

// 测试频繁交替的push和pop
void stress_test() {
    std::cout << "=== 压力测试 ===\n";
    
    ds::Queue<int> q;
    constexpr int iterations = 100000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 交替进行push和pop
    for (int i = 0; i < iterations; ++i) {
        q.push(i);
        int val;
        q.try_pop(val);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    std::cout << "压力测试完成！在 " << duration << "ms 内完成了 " << iterations << " 次push/pop操作\n";
}

namespace ds{

int test_queue() {
    try {
        basic_test();
        custom_type_test();
        thread_safety_test();
        large_object_test();
        stress_test();
        
        std::cout << "\n所有测试通过！\n";
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

}

