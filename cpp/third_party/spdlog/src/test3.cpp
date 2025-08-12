// 多Sinks Logger (同时输出到控制台和文件)
// 这是一个非常常见的需求：在开发时既要在控制台看到实时输出，
// 又要将所有日志记录到文件中以备后续分析。

#include <iostream>

// 包含需要的sink头文件
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

int main() {
    try {
        // 1. 创建 sinks 列表
        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        // true表示清空旧文件
        sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/test3.log"));

        // 2. 使用 sinks 列表创建 logger
        auto combined_logger =
            std::make_shared<spdlog::logger>("combined_logger", begin(sinks), end(sinks));

        // 3. 注册 logger 以便全局访问 (可选)
        spdlog::register_logger(combined_logger);

        // 4. 设置日志级别和格式
        combined_logger->set_level(spdlog::level::info);
        combined_logger->info("This message goes to both console and file.");
        combined_logger->warn("So does this one.");

        // 使用
        auto logger = spdlog::get("combined_logger");
        logger->error("Error message logged.");

    } catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
    }

    return 0;
}