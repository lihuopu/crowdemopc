#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace user_service::util {

class Logger {
public:
    static Logger& getInstance();
    
    void init();
    void shutdown();
    
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
    void debug(const std::string& message);

private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    std::string getCurrentTimestamp();
    std::string formatMessage(const std::string& level, const std::string& message);
};

// 便捷宏定义
#define LOG_INFO(msg) user_service::util::Logger::getInstance().info(msg)
#define LOG_WARN(msg) user_service::util::Logger::getInstance().warn(msg)
#define LOG_ERROR(msg) user_service::util::Logger::getInstance().error(msg)
#define LOG_DEBUG(msg) user_service::util::Logger::getInstance().debug(msg)

} // namespace user_service::util 