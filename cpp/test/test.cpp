#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace quick_sort {
int MedianThree(vector<int>& nums, int left, int mid, int right) {
    int l{nums[left]}, m{nums[mid]}, r{nums[right]};
    if ((m <= l && l <= r) || (r <= l && l <= m)) {
        return left;
    } else if ((m <= r && r <= l) || (l <= r && r <= m)) {
        return right;
    } else {
        return mid;
    }
}

// [left, right]
int Partition(vector<int>& nums, int left, int right) {
    int pivot_idx = MedianThree(nums, left, left + (right - left) / 2, right);
    int pivot = nums[pivot_idx];
    std::swap(nums[pivot_idx], nums[right]);  // 每次选最右边那个作为pivot
    int i = 0;                                // <pivot区间后一个
    for (int j = 0; j < right; ++j) {         // 不考虑最后一个
        if (nums[j] < pivot) {
            std::swap(nums[i], nums[j]);
            ++i;  // 右移动i
        }
    }
    // 最后交换pivot(就是right)来到中间
    std::swap(nums[i], nums[right]);
    return i;
}

// 尾递归优化
void Sort(vector<int>& nums, int left, int right) {
    while (left < right) {
        int p = Partition(nums, left, right);
        if (p - left < right - p) {
            // 只递归小的区间
            Sort(nums, left, p - 1);
            // 循环处理大区间
            left = p + 1;
        } else {
            Sort(nums, p + 1, right);
            right = p - 1;
        }
    }
}

}  // namespace quick_sort

namespace merge_sort {

// [left, mid] [mid+1, right] 合并
void Merge(vector<int>& nums, int left, int mid, int right) {
    int k = 0;
    int n = right - left + 1;
    vector<int> copy(n);
    int i = left, j = mid + 1;
    while (i <= mid && j <= right) {
        if (nums[i] < nums[j]) {  // 稳定
            copy[k++] = nums[i++];
        } else {
            copy[k++] = nums[j++];
        }
    }
    while (i <= mid) {
        copy[k++] = nums[i++];
    }
    while (j <= right) {
        copy[k++] = nums[j++];
    }
    for (int p = 0; p < n; ++p) {
        nums[left + p] = copy[p];
    }
}

void Sort(vector<int>& nums, int left, int right) {
    if (left >= right) {
        return;
    }
    int mid = left + (right - left) / 2;
    Sort(nums, left, mid);
    Sort(nums, mid + 1, right);
    Merge(nums, left, mid, right);
}

}  // namespace merge_sort

int main() {
    vector<int> nums{3, 8, 4, 6, 9, 2, 7};
    // quick_sort::Sort(nums, 0, 6);
    merge_sort::Sort(nums, 0, 6);
    for (auto& x : nums) {
        cout << x << ' ';
    }
    cout << '\n';
}