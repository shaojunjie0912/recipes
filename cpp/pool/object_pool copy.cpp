#include <cstddef>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <new>
#include <vector>

// NOTE: 侵入式链表 (union 实现)
// 当内存块空闲时，它是一个指向下一个空闲节点的指针
// 当被使用时，它是一段「未初始化的内存」，用于存储对象 T
// NOTE: 内存对齐
// 默认: max(alignof(std::byte[...]), alignof(Node*)) 即 max(1, 8) = 8
// 但是 sizeof(T) 可能大于 8

// NOTE: 思考: 对于之前的版本, 析构函数 default 只负责释放池自身占用的内存(vector 管理)
// 不会、也无法自动为用户已分配但未归还的对象调用析构函数和 Deallocate

// TODO: 版本迭代历史

// v1: 使用 unique_ptr + 自定义 T 类型的删除器, 为了能在对象池的生命周期结束时
// 不仅能释放池本身的内存, 也能在此之前先析构 T 类型的对象

// v2: 使用 shared_ptr + enabled_shared_from_this
// 为了确保对象池的存活时间至少和所有从它分配出去的对象的存活时间一样长
// 否则会出现对象池已经销毁了(pool_=nullptr), 但是却在删除器中调用了 pool_ 指针

template <typename T, size_t N>
class ObjectPool : public std::enable_shared_from_this<ObjectPool<T, N>> {
private:
    // 侵入型链表
    union Node {
        Node* next;
        alignas(T) std::byte storage[sizeof(T)];  // 按照 sizeof(T) 进行内存对齐
    };

    // 存储所有节点的连续内存
    std::vector<Node> pool_;  // NOTE: 本来就要在堆上分配, 那直接 Node 别 void* 了

    // 指向空闲链表的头部
    Node* free_list_head_{nullptr};

    mutable std::mutex mtx_;

private:
    // 自定义删除器 (有状态!)
    // NOTE: Deleter 现在持有 shared_ptr 来延长池的生命周期
    struct Deleter {
        std::shared_ptr<ObjectPool> pool_;  // TODO: 绑定相应的池

        void operator()(T* p) const {
            if (p) {
                // NOTE: 现在调用 Deallocate 是绝对安全的
                pool_->Deallocate(p);
            }
        }
    };

public:
    using UniquePtr = std::unique_ptr<T, Deleter>;

    static std::shared_ptr<ObjectPool> Create() {
        // NOTE: std::make_shared 无法访问私有构造函数, 而 new 可以
        return std::shared_ptr<ObjectPool>{new ObjectPool{}};
    }

    ~ObjectPool() {
        // 可选：在析构时进行检查，确保所有对象都已归还
        // 这在调试时非常有用
        size_t free_count = 0;
        for (Node* p = free_list_head_; p != nullptr; p = p->next) {
            free_count++;
        }
        if (free_count != N) {
            // 在实际项目中，这里可以使用日志库或断言
            std::cerr << "Warning: ObjectPool destroyed with " << (N - free_count)
                      << " objects still in use." << std::endl;
        }
    }

    // 禁止拷贝和移动
    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;

private:
    // 私有构造函数, 强制使用 Create
    ObjectPool() {
        // 预分配所有节点的内存
        pool_.resize(N);

        // 串联链表节点
        for (size_t i = 0; i < N - 1; ++i) {
            pool_[i].next = &pool_[i + 1];
        }
        pool_[N - 1].next = nullptr;

        // 初始化空闲链表头指针
        free_list_head_ = &pool_[0];
    }

public:
    // 分配一个对象
    // 使用 placement new 在获取的内存上构造对象
    template <typename... Args>
    [[nodiscard]] UniquePtr Allocate(Args&&... args) {
        Node* curr_node{nullptr};
        {
            std::lock_guard lk{mtx_};
            // TODO: 扩容策略?
            if (free_list_head_ == nullptr) {
                throw std::bad_alloc{};
            }

            curr_node = free_list_head_;              // 获取头节点
            free_list_head_ = free_list_head_->next;  // 更新头节点
        }

        // C++20 std::construct_at 替代 placement new
        T* p_obj = std::construct_at(reinterpret_cast<T*>(&curr_node->storage),
                                     std::forward<Args>(args)...);

        // NOTE: Deleter 对象传入 ObjectPool shared_from_this 指针
        // NOTE: this-> 将名称查找推迟到第二阶段: 模板实例化时
        return UniquePtr{p_obj, Deleter{this->shared_from_this()}};
    }

    // 归还一个对象
    void Deallocate(T* p) {
        if (p == nullptr) {
            return;
        }

        // NOTE: 显式调用析构函数 C++20 std::destroy_at
        std::destroy_at(p);

        // NOTE: 析构后, 内存还给池, 被 vector 管理

        // 将对象的内存重新解释为 Node 指针
        Node* node = reinterpret_cast<Node*>(p);
        {
            std::lock_guard lk{mtx_};
            // 将归还的节点插回空闲链表头部
            node->next = free_list_head_;
            free_list_head_ = node;
        }
    }
};

// --- 测试 ---
struct MyObject {
    int id;
    char data[8];

    MyObject(int i) : id(i) {
        std::cout << "MyObject(" << id << ") constructed at " << this << std::endl;
    }

    ~MyObject() { std::cout << "MyObject(" << id << ") destructed at " << this << std::endl; }
};

int main() {
    // 声明一个 unique_ptr，它将持有来自内部作用域池的对象。
    // 我们明确指定它的类型，以确保它与 temp_pool 的类型一致。
    ObjectPool<MyObject, 3>::UniquePtr p1;

    std::cout << "--- Demonstrating Deleter lifetime extension ---" << std::endl;
    {
        // 创建一个生命周期限定在此作用域内的 temp_pool
        auto temp_pool = ObjectPool<MyObject, 3>::Create();
        std::cout << "temp_pool (use count: " << temp_pool.use_count()
                  << ") created in inner scope." << std::endl;

        // 从 temp_pool 分配两个对象
        p1 = temp_pool->Allocate(1);
        auto p2 = temp_pool->Allocate(2);  // p2 的生命周期完全在此作用域内

        // Deleter 会增加 shared_ptr 的引用计数
        std::cout << "After allocation, temp_pool use count is now: " << temp_pool.use_count()
                  << " (base=1, p1= +1, p2=+1)" << std::endl;

        std::cout << "--- temp_pool's shared_ptr is about to go out of scope ---" << std::endl;
    }  // 到达作用域结尾:
       // 1. p2 被销毁。它的Deleter被调用，归还内存给池，并释放对池的shared_ptr引用。
       // 2. temp_pool 句柄被销毁，释放它对池的shared_ptr引用。

    // 此时，temp_pool 的原始句柄已销毁，但对象池本身因为 p1 仍然存在而活着。
    // 我们可以通过 p1 的 Deleter 内部的 shared_ptr 来证明这一点。
    std::cout << "Outside the scope, the pool object is still alive because of p1." << std::endl;
    std::cout << "--- p1 is about to be reset ---" << std::endl;

    p1.reset();  // p1 被销毁。它的Deleter被调用，归还内存。
                 // 这是对池的最后一个 shared_ptr 引用，因此池对象本身现在也被安全销毁。

    std::cout << "--- End of main ---" << std::endl;
    return 0;
}