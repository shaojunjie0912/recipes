#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

struct State {
    std::mutex mtx_;
    std::condition_variable cv_;
    int turn_{0};  // 当前顺序
};

const int round = 5;

int main() {
    State state;
    auto func = [&](int my_turn, char ch, State* st) {
        for (int i = 0; i < round; ++i) {
            std::unique_lock lk{st->mtx_};
            st->cv_.wait(lk, [&] { return st->turn_ == my_turn; });
            std::cout << ch << std::flush;
            // 更新顺序
            st->turn_ = (my_turn + 1) % 3;
            st->cv_.notify_all();
        }
    };
    std::jthread t1{[&] { func(0, 'A', &state); }};
    std::jthread t2{[&] { func(1, 'B', &state); }};
    std::jthread t3{[&] { func(2, 'C', &state); }};
}