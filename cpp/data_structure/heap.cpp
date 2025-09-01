#include <algorithm>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <vector>

using namespace std;

template <typename T, typename Compare = std::less<T>>
class Heap {
public:
    Heap() = default;

    explicit Heap(vector<T> const& data) : data_(data) {
        BuildHeap();  // NOTE: 记得建堆
    }

public:
    size_t Size() const { return data_.size(); }

    bool Empty() const { return Size() == 0; }

    // 插入一个元素 (从堆底向上堆化) NOTE: SiftUp
    void Push(T const& val) {
        data_.push_back(val);
        SiftUp(Size() - 1);
    }

    // 弹出堆顶元素 (交换->弹出旧堆顶->新堆顶向下堆化) NOTE: SiftDown
    void Pop() {
        // 处理空堆
        if (Empty()) {
            throw std::runtime_error("Heap is Empty!");
        }
        std::swap(data_.front(), data_.back());
        data_.pop_back();  // NOTE: 别忘记弹出!!
        // 移除后可能变成空堆
        if (!Empty()) {
            SiftDown(0);
        }
    }

    // 堆顶元素
    // NOTE: 返回值用常量引用
    T const& Top() const {
        if (Empty()) {
            throw std::runtime_error("Heap is Empty!");
        }
        return data_[0];
    }

private:
    //  建堆 (从无序数组) NOTE: SiftDown
    // 从后往前找非叶节点, 依次向下堆化 NOTE: 叶子节点已经是堆了, 所以不需要考虑
    // 第一个非叶节点: 最后一个叶子节点的父节点
    // 时间复杂度: O(n) 不是 O(nlogn) NOTE: O(n) 有数学证明, 底层节点数量>顶层
    void BuildHeap() {
        for (int i = Parent(Size() - 1); i >= 0; --i) {
            SiftDown(i);
        }
    }

private:
    // 自底至顶堆化 (比较父节点和节点 i)
    void SiftUp(int i) {
        // NOTE: compare(a, b)
        // 返回 true 说明 a 优先级低, a 沉下去; 返回 false 说明 a 优先级高, a 浮上来
        while (i > 0 && cmp_(data_[Parent(i)], data_[i])) {  // NOTE: 索引 i > 0 不能越界
            std::swap(data_[Parent(i)], data_[i]);
            i = Parent(i);
        }
    }

    // 自顶置底堆化 (比较 i 和 l r 左右子节点, 与最大的交换)
    void SiftDown(int i) {
        while (true) {
            int max = i;  // 初始化 up 为 i (up 表示需要上浮)
            int l = Left(i), r = Right(i);
            // 跟 data_[l] 比较看是否更新 up NOTE: 子节点不能越界
            if (l < Size() && cmp_(data_[max], data_[l])) {  // NOTE: 这里是跟 nums[max] 比
                max = l;
            }
            // 跟 data_[r] 比较看是否更新 up // NOTE: 子节点不能越界
            if (r < Size() && cmp_(data_[max], data_[r])) {  // NOTE: 这里是跟 nums[max] 比
                max = r;
            }
            if (max == i) {  // 如果 up 没有被更新还是 i 则堆化结束
                break;
            }
            std::swap(data_[i], data_[max]);  // data_[i] 与需要上浮的交换
            i = max;                          // 更新 i 进行下一轮
        }
    }

private:
    // [完全二叉树]左子节点索引
    size_t Left(size_t i) const { return 2 * i + 1; }

    // [完全二叉树]右子节点索引
    size_t Right(size_t i) const { return 2 * i + 2; }

    // [完全二叉树]父节点索引
    size_t Parent(size_t i) const { return (i - 1) / 2; }

private:
    vector<T> data_;
    Compare cmp_;
};