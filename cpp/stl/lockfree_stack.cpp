#include <atomic>
#include <iostream>
#include <memory>
#include <optional>
#include <thread>
#include <vector>

template <typename T>
class LockFreeStack {
public:
    LockFreeStack() = default;
    ~LockFreeStack() = default;
    LockFreeStack(LockFreeStack const&) = delete;
    LockFreeStack(LockFreeStack&&) = delete;

public:
    void Push(T data) {
        auto new_node{std::make_shared<Node>(std::move(data))};
        new_node->next_ = head_.load(std::memory_order_acquire);
        // NOTE: 当前值: new_node->next_
        // Push CAS 成功: release语义, 需要把 new_node 写入 head
        // Push CAS 失败:  acquire语义, 需要读取 head_ 的最新值到 new_node->next_
        while (!head_.compare_exchange_weak(new_node->next_, new_node, std::memory_order_release,
                                            std::memory_order_acquire)) {
            // CAS 失败时, new_node->next_ 已被自动更新为 head_ 的最新值(涉及读取),
            // 只需简单重试即可
        }
    }

    std::optional<T> Pop() {
        auto old_head{head_.load(std::memory_order_acquire)};  // 引用计数 +1 变成 2
        // NOTE: old_head 非空, 当前值: old_head
        // Pop CAS 成功: acq-rel语义, 需要把old_head->next_写入head + 后面会读取old_head的data_
        // Pop CAS 失败:  acquire语义, 需要读取 head_ 的最新值到 old_head
        while (old_head &&
               !head_.compare_exchange_weak(old_head, old_head->next_, std::memory_order_acq_rel,
                                            std::memory_order_acquire)) {  // 引用计数 -1 变成 1
            // CAS 失败时, old_head 已被自动更新为 head_ 的最新值(涉及读取)
            // 我们只需继续循环。
        }
        // NOTE: 判断空链表
        if (old_head) {
            return old_head->data_;  // 读取节点内容
        }
        return std::nullopt;
    }

private:
    // 内部定义
    struct Node {
        T data_;
        std::shared_ptr<Node> next_{nullptr};
        // 一个节点被创建时，它的 next_ 指针的意图是稍后被设置，而不是在构造时。
        // 一个更清晰的构造函数只接受它所包含的数据。
        explicit Node(T data) : data_(std::move(data)) {}
    };

private:
    std::atomic<std::shared_ptr<Node>> head_;
};

int main() {
    LockFreeStack<int> stack;
    std::atomic<int> sum_popped = 0;
    std::atomic<int> pop_count = 0;
    const int kItemsPerProducer = 2000;
    const int kProducerCount = 4;
    const int kConsumerCount = 4;
    const int kTotalItems = kItemsPerProducer * kProducerCount;

    std::vector<std::jthread> producers;
    for (int i = 0; i < kProducerCount; ++i) {
        producers.emplace_back([&stack, i] {
            for (int j = 0; j < kItemsPerProducer; ++j) {
                // 每个生产者产生不同的数字范围，以确保数据唯一
                stack.Push(i * kItemsPerProducer + j);
            }
        });
    }

    std::vector<std::jthread> consumers;
    for (int i = 0; i < kConsumerCount; ++i) {
        consumers.emplace_back([&stack, &sum_popped, &pop_count] {
            while (pop_count.load() < kTotalItems) {
                auto value = stack.Pop();
                if (value.has_value()) {
                    sum_popped += *value;
                    pop_count++;
                }
                // 在没有取到值时稍微让出CPU，避免空转
                // 在高竞争下这是一种常见的策略
                else {
                    std::this_thread::yield();
                }
            }
        });
    }

    // producers 和 consumers 的 jthread 会在析构时自动 join

    std::cout << "所有生产者和消费者线程已完成。\n";
    std::cout << "总共压入的元素数量: " << kTotalItems << "\n";
    std::cout << "总共弹出的元素数量: " << pop_count.load() << "\n";
    std::cout << "弹出元素的总和: " << sum_popped.load() << "\n";

    // 计算期望的总和：0 + 1 + 2 + ... + (kTotalItems - 1)
    long long expected_sum = (long long)(kTotalItems - 1) * kTotalItems / 2;
    std::cout << "期望的总和: " << expected_sum << "\n";

    if (sum_popped.load() == expected_sum && pop_count.load() == kTotalItems) {
        std::cout << "测试成功！\n";
    } else {
        std::cout << "测试失败！\n";
    }

    return 0;
}