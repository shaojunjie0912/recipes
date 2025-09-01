#include <condition_variable>
#include <mutex>

// 写者优先: 这里的「优先」并不意味着写者可以“插队”
// 而是: 当一个写者线程正在等待时, 新的读者线程将不会被授权, 以防止写者饥饿

// 写者状态: 活跃 | 等待(有专门的等待数量记录)
// 读者状态: 活跃

class RWLock {
private:
    int reader_count_{0};        // 「活跃」的读者数量
    int waiting_writers_{0};     // 「等待」的写者数量
    bool writer_active_{false};  // 是否有活跃写者

    std::mutex mtx_;
    std::condition_variable cv_can_read_;   // 「读者可以读」
    std::condition_variable cv_can_write_;  // 「写者可以写」

public:
    RWLock() = default;
    ~RWLock() = default;
    RWLock(RWLock const &) = delete;
    RWLock(RWLock &&) = delete;

public:
    // ========================= 读上锁/解锁 =========================
    // 获取读锁
    void ReadLock() {
        std::unique_lock lk{mtx_};
        // NOTE: 「写者优先」这里体现!
        // 读者必须等待直到以下所有条件成立才可以读
        //  * 没有正在活跃的写者 false
        //  * 没有等待写者 == 0
        cv_can_read_.wait(lk, [this] { return !writer_active_ && waiting_writers_ == 0; });
        ++reader_count_;  // ReadLock 和 ReadUnlock 是一个组合, 在 ReadUnlock 最后 notify
    }

    // 释放读锁
    void ReadUnlock() {
        std::lock_guard lk{mtx_};
        --reader_count_;
        // NOTE: 唤醒写者是有条件的! 首先读者已经没了, 其次确实存在等待的写者, 不然也没必要唤醒
        // 如果这是最后一个离开的读者, 并且有写者在等待, 则唤醒一个写者
        if (reader_count_ == 0 && waiting_writers_ > 0) {
            cv_can_write_.notify_one();
        }
    }
    // ===============================================================

    // ========================= 写上锁/解锁 =========================
    // 获取写锁
    void WriteLock() {
        std::unique_lock lk{mtx_};
        ++waiting_writers_;  // NOTE: 进入等待队列, 不是直接变成活跃, 因为还没获得锁!!
        // 写者必须等待直到以下所有条件成立才可以写(体现: 写-读/写互斥)
        //  * 没有正在活跃的写者 false
        //  * 没有活跃读者 == 0
        cv_can_write_.wait(lk, [this] { return !writer_active_ && reader_count_ == 0; });
        writer_active_ = true;  // 变成活跃写者
        --waiting_writers_;     // WriteLock 和 WriteUnlock 是一个组合, 在 WriteUnlock 最后 notify
    }

    // 释放写锁
    void WriteUnlock() {
        std::lock_guard lk{mtx_};
        writer_active_ = false;
        // 「写者优先」
        // 如果等待队列有等待的写者, 则优先唤醒一个写者
        if (waiting_writers_ > 0) {
            cv_can_write_.notify_one();  // NOTE: 一个写者
        } else {
            // 否则唤醒所有等待的读者
            cv_can_read_.notify_all();  // NOTE: 所有读者
        }
    }
    // ===============================================================
};

int main() {}