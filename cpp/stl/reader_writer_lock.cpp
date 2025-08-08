#include <condition_variable>
#include <mutex>

// 写者优先: 当一个写者线程正在等待时, 新的读者线程将不会被授权, 以防止写者饥饿

// 写者状态: 等待 | 活跃
// 读者状态: 活跃 (没有等待队列)

class ReaderWriterLock {
public:
    ReaderWriterLock() = default;
    ~ReaderWriterLock() = default;
    // 禁止拷贝和移动
    ReaderWriterLock(ReaderWriterLock const &) = delete;
    ReaderWriterLock(ReaderWriterLock &&) = delete;

public:
    // 获取读锁
    void ReadLock() {
        std::unique_lock lk{mtx_};
        // NOTE: 「写者优先」这里体现!
        // 读者必须等待直到
        //  * 没有等待写者
        //  * 没有正在活跃的写者
        cv_read_.wait(lk, [this] { return waiting_writers_ == 0 && !writer_active; });
        ++reader_count_;
    }

    // 释放读锁
    void ReadUnlock() {
        std::lock_guard lk{mtx_};
        --reader_count_;
        // 如果这是最后一个离开的读者, 并且有写者在等待, 则唤醒一个写者
        if (reader_count_ == 0 && waiting_writers_ > 0) {
            cv_write_.notify_one();
        }
    }

    // 获取写锁
    void WriteLock() {
        std::unique_lock lk{mtx_};
        ++waiting_writers_;  // NOTE: 进入等待队列
        // 写者必须等待直到
        //  * 没有活跃读者
        //  * 没有正在活跃的写者
        cv_write_.wait(lk, [this] { return reader_count_ == 0 && !writer_active; });
        --waiting_writers_;  // 将要变为活跃写者，结束等待计数
        writer_active = true;
    }

    // 释放写锁
    void WriteUnlock() {
        std::lock_guard lk{mtx_};
        writer_active = false;
        // 「写者优先」
        // 如果等待队列有等待的写者, 则优先唤醒一个写者
        if (waiting_writers_ > 0) {
            cv_write_.notify_one();
        } else {
            // 否则唤醒所有等待的读者
            cv_read_.notify_all();
        }
    }

private:
    std::mutex mtx_;                    // 保护内部状态的互斥锁
    std::condition_variable cv_read_;   // 读者等待的条件变量
    std::condition_variable cv_write_;  // 写者等待的条件变量

    int reader_count_{0};       // 「正在活跃」的读者数量
    int waiting_writers_{0};    // 「正在等待」的写者数量
    bool writer_active{false};  // 是否有写者正在活跃
};

int main() {}