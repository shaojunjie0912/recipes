#include <cstdint>   // For uintptr_t
#include <cstdlib>   // For malloc and free
#include <iostream>  // For std::cout

/**
 * @brief 分配指定大小且地址对齐的内存。
 *
 * @param size 要分配的字节数(必须是 alignment 的整数倍, std::aligned_alloc)
 * @param alignment 对齐字节数，必须是2的幂。
 * @return void* 成功时返回对齐后的内存指针，失败时返回 nullptr。
 */
void* AlignedAlloc(size_t alignment, size_t size) {
    // 检查 alignment 是否是 2 的幂，这是一个高效的位运算技巧。
    if ((alignment & (alignment - 1)) != 0) {
        return nullptr;
    }

    // 1. 计算需要分配的总内存大小。
    //    - size: 用户实际需要的大小。
    //    - alignment - 1: 为了保证能找到一个对齐地址，最多需要的额外空间。
    //    - sizeof(void*): 用来存储 malloc 返回的原始指针。
    const size_t total_size = size + (alignment - 1) + sizeof(void*);

    // 2. 使用 malloc 分配原始内存块。
    void* p_orig = malloc(total_size);
    if (p_orig == nullptr) {
        return nullptr;  // 内存分配失败。
    }

    // 3. 在分配的内存中找到对齐的地址。
    //    - 将指针转换为整数类型 uintptr_t 以便进行数学运算。
    //    - (p_original_addr + sizeof(void*) + alignment - 1):
    //      确保在为原始指针留出空间后，还能找到对齐地址。
    //    - & ~(alignment - 1): 将地址向下舍入到最近的对齐边界。
    const uintptr_t p_orig_addr = reinterpret_cast<uintptr_t>(p_orig);
    const uintptr_t p_align_addr = (p_orig_addr + sizeof(void*) + alignment - 1) & ~(alignment - 1);
    void* p_align = reinterpret_cast<void*>(p_align_addr);

    // 4. 在对齐地址的前面存储原始指针。
    //    - 将对齐指针向前移动一个 void* 的位置。
    //    - `(void**)p_aligned` 将其转换为一个指向 void* 的指针。
    //    - `[-1]` 操作符访问前一个元素。
    ((void**)p_align)[-1] = p_orig;

    // 5. 返回对齐后的指针给用户。
    return p_align;
}

/**
 * @brief 释放由 AlignedAlloc 分配的内存。
 *
 * @param p_aligned AlignedAlloc 返回的指针。
 */
void AlignedFree(void* p_aligned) {
    if (p_aligned == nullptr) {
        return;
    }

    // 1. 从对齐地址的前面找回原始指针。
    void* p_original = ((void**)p_aligned)[-1];

    // 2. 释放原始的、完整的内存块。
    free(p_original);
}

// === 测试代码 ===
struct MyData {
    int id;
    char buffer[13];  // 结构体大小不是8的倍数，便于测试
};

int main() {
    const size_t kAlignment = 8;
    std::cout << "请求以 " << kAlignment << " 字节对齐的内存。" << std::endl;

    // 测试1: 分配一个结构体
    std::cout << "\n--- 测试 1: 分配 MyData 结构体 ---" << std::endl;
    MyData* data = static_cast<MyData*>(AlignedAlloc(sizeof(MyData), kAlignment));

    if (data != nullptr) {
        // 将指针转换为整数地址以便验证
        uintptr_t address = reinterpret_cast<uintptr_t>(data);
        std::cout << "分配的地址: 0x" << std::hex << address << std::dec << std::endl;
        std::cout << "地址 " << address << " % " << kAlignment << " = " << (address % kAlignment)
                  << std::endl;

        if ((address % kAlignment) == 0) {
            std::cout << "验证成功: 地址是 " << kAlignment << " 的倍数。" << std::endl;
        } else {
            std::cout << "验证失败: 地址不是 " << kAlignment << " 的倍数。" << std::endl;
        }

        // 使用内存
        data->id = 100;

        // 释放内存
        AlignedFree(data);
        std::cout << "内存已释放。" << std::endl;
    } else {
        std::cout << "内存分配失败！" << std::endl;
    }

    // 测试2: 分配一个char数组
    std::cout << "\n--- 测试 2: 分配 99 字节的 char 数组 ---" << std::endl;
    char* buffer = static_cast<char*>(AlignedAlloc(99, kAlignment));
    if (buffer != nullptr) {
        uintptr_t address = reinterpret_cast<uintptr_t>(buffer);
        std::cout << "分配的地址: 0x" << std::hex << address << std::dec << std::endl;
        std::cout << "地址 " << address << " % " << kAlignment << " = " << (address % kAlignment)
                  << std::endl;

        if ((address % kAlignment) == 0) {
            std::cout << "验证成功: 地址是 " << kAlignment << " 的倍数。" << std::endl;
        } else {
            std::cout << "验证失败: 地址不是 " << kAlignment << " 的倍数。" << std::endl;
        }

        // 释放内存
        AlignedFree(buffer);
        std::cout << "内存已释放。" << std::endl;
    } else {
        std::cout << "内存分配失败！" << std::endl;
    }

    return 0;
}