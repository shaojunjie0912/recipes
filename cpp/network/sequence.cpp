#include <arpa/inet.h>
#include <unistd.h>

#include <algorithm>
#include <bit>  // C++20, for std::endian
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <span>  // C++20
#include <stdexcept>
#include <type_traits>  // For std::is_trivially_copyable_v
#include <vector>

// 定义协议头结构体
#pragma pack(push, 1)
struct ProtocolHeader {
    uint8_t version;
    uint8_t type;
    uint16_t seq_num;
    uint32_t timestamp;
    uint32_t length;
};
#pragma pack(pop)

// [优化 2] 编译时安全检查：确保 ProtocolHeader 适用于底层内存操作
static_assert(std::is_trivially_copyable_v<ProtocolHeader>,
              "ProtocolHeader must be a trivially copyable type for safe serialization.");

// [优化 1 & 4] 序列化: 提供一个写入 span 的高性能版本
// 返回写入的字节数
size_t SerializeTo(const ProtocolHeader& header, std::span<std::byte> target_buffer) {
    if (target_buffer.size() < sizeof(ProtocolHeader)) {
        throw std::runtime_error("Target buffer is too small for serialization.");
    }

    // 复制结构体，避免修改原始输入
    ProtocolHeader header_net = header;

    // [优化 3] 统一使用标准字节序转换函数
    header_net.seq_num = htons(header.seq_num);
    header_net.timestamp = htonl(header.timestamp);
    header_net.length = htonl(header.length);

    // 使用 C++20 的 std::as_bytes 获取一个安全的字节视图
    auto bytes_view = std::as_bytes(std::span{&header_net, 1});
    std::copy(bytes_view.begin(), bytes_view.end(), target_buffer.begin());

    return bytes_view.size();
}

// 序列化: 保留返回 vector 的便捷版本
std::vector<std::byte> Serialize(const ProtocolHeader& header) {
    std::vector<std::byte> buffer(sizeof(ProtocolHeader));
    SerializeTo(header, buffer);  // 复用高性能版本
    return buffer;
}

// [优化 1 & 4] 反序列化: 接受 std::span，API更通用，逻辑更简洁
ProtocolHeader Deserialize(std::span<const std::byte> buffer_view) {
    if (buffer_view.size() < sizeof(ProtocolHeader)) {
        throw std::runtime_error("Buffer view is too small for deserialization.");
    }

    ProtocolHeader header;
    // 直接将数据复制到最终的结构体中
    std::memcpy(&header, buffer_view.data(), sizeof(ProtocolHeader));

    // 就地进行字节序转换
    header.seq_num = ntohs(header.seq_num);
    header.timestamp = ntohl(header.timestamp);
    header.length = ntohl(header.length);

    return header;
}

int main() {
    ProtocolHeader original_header = {1, 0x0A, 1024, 987654321, 4096};

    // --- 使用返回 vector 的版本 ---
    std::vector<std::byte> packet_data = Serialize(original_header);

    std::cout << "Serialized packet data (size: " << packet_data.size() << " bytes):" << std::endl;
    for (const auto& b : packet_data) {
        std::cout << std::hex << std::to_integer<int>(b) << " ";
    }
    std::cout << std::dec << std::endl << std::endl;

    // --- 反序列化时，vector 可以隐式转换为 span ---
    try {
        // [优化 1 的体现] Deserialize 现在接受任何连续内存，包括 vector
        ProtocolHeader deserialized_header = Deserialize(packet_data);

        std::cout << "Deserialized Header from vector:" << std::endl;
        std::cout << "  Version: " << static_cast<int>(deserialized_header.version) << std::endl;
        std::cout << "  Type: 0x" << std::hex << static_cast<int>(deserialized_header.type)
                  << std::dec << std::endl;
        std::cout << "  Sequence: " << deserialized_header.seq_num << std::endl;
        std::cout << "  Timestamp: " << deserialized_header.timestamp << std::endl;
        std::cout << "  Length: " << deserialized_header.length << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    // --- 使用写入 span 的高性能版本 ---
    std::cout << "\n--- Testing high-performance serialization ---" << std::endl;
    std::byte c_style_buffer[sizeof(ProtocolHeader)];
    size_t bytes_written = SerializeTo(original_header, c_style_buffer);

    // [优化 1 的体现] Deserialize 现在也能直接处理 C 风格数组
    ProtocolHeader deserialized_from_c_array = Deserialize(c_style_buffer);
    std::cout << "Deserialized Header from C-style array:" << std::endl;
    std::cout << "  Sequence: " << deserialized_from_c_array.seq_num << std::endl;

    return 0;
}