// 创建和使用自定义Logger
// 对于大型项目或库，创建具名的、独立的Logger是更好的实践，
// 这样可以避免和全局Logger冲突，并能对不同模块进行精细化控制。

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <iostream>

int main() {
    // 创建一个彩色的、多线程安全的控制台logger
    // 使用 std::make_shared 来创建 sink 和 logger
    auto console_logger = spdlog::stdout_color_mt("console_logger");

    console_logger->info("This is from the custom console logger.");
    console_logger->set_level(spdlog::level::trace);
    console_logger->trace("This is a trace message.");

    // 获取已创建的logger
    auto existing_logger = spdlog::get("console_logger");
    if (existing_logger) {
        existing_logger->warn("Got the logger and logged a warning.");
    }

    // 使用完毕后，可以手动释放logger
    // spdlog::drop("console_logger");
}