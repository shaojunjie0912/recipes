#include <cstdint>
#include <iostream>
#include <vector>

// 为了代码整洁，我们使用类型别名
using Matrix = std::vector<std::vector<int64_t>>;

int64_t Multiply(const Matrix& mat_a, const Matrix& mat_b) {
    const size_t n = mat_a.size();
    // 处理空矩阵的边界情况
    if (n == 0) {
        return 0;
    }

    // 创建结果矩阵 C，并初始化为 0
    Matrix mat_c(n, std::vector<int64_t>(n, 0));

    // 使用 i-k-j 循环顺序进行缓存优化
    // 这种顺序最大化了内层循环中内存访问的连续性
    for (size_t i = 0; i < n; ++i) {
        for (size_t k = 0; k < n; ++k) {
            // 将 A[i][k] 的值缓存在一个寄存器变量中，以加速内层循环
            const int64_t r = mat_a[i][k];
            for (size_t j = 0; j < n; ++j) {
                // 内层循环中，对 B 的第 k 行和 C 的第 i 行都是连续访问，缓存友好
                mat_c[i][j] += r * mat_b[k][j];
            }
        }
    }

    // 计算结果矩阵 C 的所有元素之和
    int64_t sum = 0;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            sum += mat_c[i][j];
        }
    }

    return sum;
}