#include <iostream>
#include <memory>
#include <optional>

// 前置声明
class GeneratorStateMachine;

/**
 * @class Generator
 * @brief 用户友好的生成器接口，模拟协程返回的对象.
 *
 * 它持有一个状态机实例，并提供简单的API来驱动它并获取值.
 */
class Generator {
public:
    Generator();

    // 检查生成器是否还能产出新的值
    bool has_value() const;

    // 获取当前产出的值
    // Precondition: has_value() must be true.
    int get() const;

    // 驱动状态机到下一个状态 (产出下一个值)
    void next();

private:
    // 使用智能指针，模拟协程帧在堆上分配.
    std::unique_ptr<GeneratorStateMachine> state_machine_;
};

/**
 * @class GeneratorStateMachine
 * @brief 手动实现的状态机，模拟编译器生成的协程帧.
 */
class GeneratorStateMachine {
public:
    GeneratorStateMachine() = default;

    // 禁止拷贝和移动，因为状态机与它的状态紧密绑定.
    GeneratorStateMachine(const GeneratorStateMachine&) = delete;
    GeneratorStateMachine& operator=(const GeneratorStateMachine&) = delete;

    // 驱动状态机前进的函数，模拟 coroutine_handle::resume()
    void move_next() {
        // 状态机的核心：根据当前状态，跳转到对应的代码块执行
        switch (state_) {
            case State::kStart:
                std::cout << "[State Machine] a. Coroutine started.\n";
                // "co_yield 0;"
                current_value_ = 0;
                state_ = State::kAfterYield1;  // 更新状态，准备下次恢复
                return;                        // 暂停执行

            case State::kAfterYield1:
                std::cout << "[State Machine] b. Resumed after yielding 0.\n";
                // "co_yield 1;"
                current_value_ = 1;
                state_ = State::kAfterYield2;  // 更新状态
                return;                        // 暂停执行

            case State::kAfterYield2:
                std::cout << "[State Machine] c. Resumed after yielding 1.\n";
                // "co_yield 2;"
                current_value_ = 2;
                state_ = State::kDone;  // 更新状态
                return;                 // 暂停执行

            case State::kDone:
                std::cout << "[State Machine] d. Resumed after yielding 2, finishing.\n";
                // 协程结束，没有更多的值了
                current_value_.reset();  // 清空当前值
                return;
        }
    }

    bool has_value() const { return current_value_.has_value(); }

    int get() const { return current_value_.value(); }

private:
    // 1. 状态变量：记录协程的暂停点
    enum class State {
        kStart,        // 初始状态
        kAfterYield1,  // 在第一次 yield 后暂停
        kAfterYield2,  // 在第二次 yield 后暂停
        kDone          // 执行完毕
    };
    State state_ = State::kStart;

    // 2. "局部变量"：需要跨越暂停点保留的变量，必须成为成员变量
    // 在这个例子中没有，但如果协程里有 for (int i = 0; ...) 循环，
    // 那么 'i' 就会被保存在这里。

    // 3. "返回值"：用于存放 co_yield 出来的值
    std::optional<int> current_value_;
};

// --- Generator 成员函数实现 ---

Generator::Generator() : state_machine_(std::make_unique<GeneratorStateMachine>()) {
    // 构造时，立即驱动一次状态机，使其产出第一个值并准备好
    state_machine_->move_next();
}

bool Generator::has_value() const { return state_machine_ && state_machine_->has_value(); }

int Generator::get() const {
    // 在实际应用中，访问一个没有值的 optional 会抛出异常
    return state_machine_->get();
}

void Generator::next() {
    if (state_machine_) {
        state_machine_->move_next();
    }
}

int main() {
    std::cout << "Creating the generator...\n";
    Generator generator;
    std::cout << "Generator created. Starting consumption loop.\n\n";

    // 使用 while 循环消费生成器产出的所有值
    while (generator.has_value()) {
        std::cout << "main: Got value: " << generator.get() << "\n";
        std::cout << "main: Moving to next value...\n\n";
        generator.next();
    }

    std::cout << "Generator finished. No more values.\n";

    return 0;
}