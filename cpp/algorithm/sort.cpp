#include <iostream>
#include <random>
#include <vector>

using namespace std;

// 快速排序
namespace quick_sort {

// 随机数生成引擎
std::mt19937 gen{std::random_device{}()};

// [l, r] 闭区间选最后一个元素作为 pivot
// 以 pivot 划分区间: < pivot 和 pivot 和 >= pivot
int Partition(vector<int>& nums, int l, int r) {
    std::uniform_int_distribution<int> dist{l, r};
    std::swap(nums[dist(gen)], nums[r]);  // 把基准数移动到最右边 (因为把nums[right]作为基准数)
    int pivot = nums[r];

    // NOTE: 这里 i 初始化为 l(eft)
    int i = l;  // i: 小于 pivot 区域最后一个元素的下一个位置
    // NOTE: 这里 j 初始化为 l(eft)
    for (int j = l; j < r; ++j) {  // j 从l到r-1遍历数组(不包括r)
        // 符合 <pivot 就跟 nums[i] 交换
        if (nums[j] < pivot) {  // NOTE: 比较的是 pivot 不是nums[pivot]!!
            std::swap(nums[j], nums[i]);
            ++i;  // NOTE: 别忘了 ++i
        }
    }
    std::swap(nums[i], nums[r]);  // 最后交换pivot到中间
    return i;
}

// 优化递归深度: 模拟尾递归 [空间复杂度最坏O(n)->O(logn)]
// 小分区: 递归
// 大分区: 递归->循环
void Sort(vector<int>& nums, int left, int right) {
    // NOTE: 退出条件被while包含了 (仍然不考虑一个元素)
    while (left < right) {
        int p = Partition(nums, left, right);
        if (p - left < right - p) {    // 左边小: 递归
            Sort(nums, left, p - 1);   // NOTE: p 已经分好, 考虑 p - 1
            left = p + 1;              // 右边大: 更新左边界, 循环
        } else {                       // 右边小: 递归
            Sort(nums, p + 1, right);  // NOTE: p 已经分好, 考虑 p + 1
            right = p - 1;             // 左边大: 更新右边界, 循环
        }
    }
}

}  // namespace quick_sort

namespace merge_sort {

// 合并 [l, m] 和 [m+1, r] 两个有序数组
void Merge(vector<int>& nums, int l, int m, int r) {
    int n = r - l + 1;
    vector<int> tmp(n);  // 辅助数组 这里的 n 是 l ~ r 的长度
    int k = 0;           // NOTE: 0 ~ n - 1
    int i = l;
    int j = m + 1;
    while (i <= m && j <= r) {
        if (nums[i] < nums[j]) {
            tmp[k++] = nums[i++];
        } else {
            tmp[k++] = nums[j++];
        }
    }
    // 跟合并两个有序链表一样, 处理剩余的
    while (i <= m) {
        tmp[k++] = nums[i++];
    }
    while (j <= r) {
        tmp[k++] = nums[j++];
    }
    // tmp -> nums
    for (int i = 0; i < n; ++i) {
        nums[l + i] = tmp[i];  // NOTE: nums 数组索引偏移 l + i
    }
}

void Sort(vector<int>& nums, int l, int r) {
    if (l >= r) {  // NOTE: >= 的 = 不能少!! 不然下面变成 [l+1, l]
        return;
    }
    int m = l + (r - l) / 2;
    Sort(nums, l, m);      // [l, m]
    Sort(nums, m + 1, r);  // [m+1, r]
    Merge(nums, l, m, r);
}

}  // namespace merge_sort

namespace heap_sort {

// 1. 建大顶堆
// 2. 循环 n - 1 轮:
//    - 交换堆顶和堆底元素(首尾)
//    - 自顶至底堆化 (堆长度递减)

// n: 堆当前的有效长度 (因为每排序一个元素堆长度都会减一)
void SiftDown(vector<int>& nums, int i, int n) {
    while (true) {
        int max = i;
        int l = 2 * i + 1, r = 2 * i + 2;
        if (l < n && nums[l] > nums[max]) {  // NOTE: 这里是跟 nums[max] 比
            max = l;
        }
        if (r < n && nums[r] > nums[max]) {  // NOTE: 这里是跟 nums[max] 比
            max = r;
        }
        if (max == i) {
            break;
        }
        std::swap(nums[i], nums[max]);
        i = max;
    }
}

void Sort(vector<int>& nums) {
    int n = nums.size();
    // 从非叶子节点开始堆化 (注意Parent(i) = (i-1)/2, 这里i = n-1 表示最后一个节点)
    for (int i = n / 2 - 1; i >= 0; --i) {
        SiftDown(nums, i, n);  // NOTE: 这里长度一直是 n
    }
    // 循环 n-1 轮排序 NOTE: > 0 因为一个元素不需要排序
    for (int i = n - 1; i > 0; --i) {
        // 交换堆顶和堆底
        std::swap(nums[0], nums[i]);  // i 是堆底节点(尾)
        // 堆化
        SiftDown(nums, 0, i);  // 堆化每次从堆顶 0 开始, 堆长度要递减, 长度就是 i
    }
}

}  // namespace heap_sort

int main() {
    vector<int> nums{3, 2, 5, 6, 4, 9, 8, 10, 7};
    int n = nums.size();
    quick_sort::Sort(nums, 0, n - 1);
    for (auto& x : nums) {
        cout << x << ' ';
    }
    cout << '\n';
    merge_sort::Sort(nums, 0, n - 1);
    for (auto& x : nums) {
        cout << x << ' ';
    }
    cout << '\n';
    heap_sort::Sort(nums);
    for (auto& x : nums) {
        cout << x << ' ';
    }
    cout << '\n';
}