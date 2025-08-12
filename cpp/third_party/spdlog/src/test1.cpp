// 全局Logger与基本日志
// spdlog 提供了一个全局的、易于访问的默认Logger，无需任何设置即可使用。

#include <spdlog/spdlog.h>

#include <iostream>

int main() {
    // 使用默认的全局logger
    // 日志级别默认为 info
    spdlog::info("Welcome to spdlog!");
    spdlog::error("Some error message with arg: {}", 1);

    // 使用 {fmt} 库的格式化语法
    spdlog::warn("Easy padding in numbers like {:08d}", 12345);
    spdlog::critical("Support for floats {:03.2f} and bools {}", 3.14159, true);

    // 改变全局logger的日志级别
    // 只有等于或高于此级别的日志才会被记录
    spdlog::set_level(spdlog::level::debug);  // 设置为debug级别
    spdlog::debug("This message should appear after setting level to debug.");
}