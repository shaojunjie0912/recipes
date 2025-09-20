#include <atomic>
#include <iostream>
#include <memory>
#include <numeric>
#include <optional>
#include <thread>
#include <vector>

// C++17 版本的无锁栈实现
// 核心机制与C++20版本相同，但语法和类型声明有区别

template <typename T>
class LockFreeStack {
public:
    LockFreeStack() = default;
    ~LockFreeStack() = default;

    LockFreeStack(const LockFreeStack&) = delete;
    LockFreeStack& operator=(const LockFreeStack&) = delete;
    LockFreeStack(LockFreeStack&&) = delete;
    LockFreeStack& operator=(LockFreeStack&&) = delete;

    void push(T data) {
        auto new_node = std::make_shared<Node>(std::move(data));

        // 使用自由函数 std::atomic_load 来原子地读取 head_
        new_node->next_ = std::atomic_load(&head_);

        // 使用自由函数 std::atomic_compare_exchange_weak
        // 注意第一个参数是 head_ 的地址
        while (!std::atomic_compare_exchange_weak(&head_, &new_node->next_, new_node)) {
            // CAS 失败时，new_node->next_ 已被自动更新为 head_ 的当前值
        }
    }
    std::optional<T> pop() {
        // 使用自由函数 std::atomic_load 来原子地获取栈顶
        std::shared_ptr<Node> old_head = std::atomic_load(&head_);

        // 循环尝试更新栈顶
        while (old_head && !std::atomic_compare_exchange_weak(&head_, &old_head, old_head->next_)) {
            // CAS 失败时，old_head 会被自动更新为 head_ 的最新值
        }

        if (old_head) {
            return old_head->data_;
        }

        return std::nullopt;
    }

private:
    struct Node {
        T data_;
        std::shared_ptr<Node> next_;

        explicit Node(T data) : data_(std::move(data)), next_(nullptr) {}
    };

    // C++17 中没有 std::atomic<std::shared_ptr<T>> 特化类
    // 我们声明一个普通的 shared_ptr 成员
    // 对它的所有并发访问都必须通过 std::atomic_... 系列的自由函数来完成
    std::shared_ptr<Node> head_;
};

int main() {
    LockFreeStack<int> stack;
    std::atomic<int> sum_popped = 0;
    std::atomic<int> pop_count = 0;
    const int kItemsPerProducer = 2000;
    const int kProducerCount = 4;
    const int kConsumerCount = 4;
    const int kTotalItems = kItemsPerProducer * kProducerCount;

    std::vector<std::thread> producers;
    for (int i = 0; i < kProducerCount; ++i) {
        producers.emplace_back([&stack, i] {
            for (int j = 0; j < kItemsPerProducer; ++j) {
                stack.push(i * kItemsPerProducer + j);
            }
        });
    }

    std::vector<std::thread> consumers;
    for (int i = 0; i < kConsumerCount; ++i) {
        consumers.emplace_back([&stack, &sum_popped, &pop_count, kTotalItems] {
            while (pop_count.load(std::memory_order_relaxed) < kTotalItems) {
                auto value = stack.pop();
                if (value.has_value()) {
                    sum_popped += *value;
                    pop_count++;
                } else {
                    std::this_thread::yield();
                }
            }
        });
    }

    // C++17: 必须手动 join 所有线程
    for (auto& t : producers) {
        t.join();
    }
    for (auto& t : consumers) {
        t.join();
    }

    std::cout << "所有生产者和消费者线程已完成。\n";
    std::cout << "总共压入的元素数量: " << kTotalItems << "\n";
    std::cout << "总共弹出的元素数量: " << pop_count.load() << "\n";
    std::cout << "弹出元素的总和: " << sum_popped.load() << "\n";

    long long expected_sum = (long long)(kTotalItems - 1) * kTotalItems / 2;
    std::cout << "期望的总和: " << expected_sum << "\n";

    if (sum_popped.load() == expected_sum && pop_count.load() == kTotalItems) {
        std::cout << "测试成功！\n";
    } else {
        std::cout << "测试失败！\n";
    }

    return 0;
}