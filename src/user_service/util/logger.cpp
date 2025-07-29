#include "logger.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace user_service::util {

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::init() {
    info("用户服务日志系统初始化完成");
}

void Logger::shutdown() {
    info("用户服务日志系统关闭");
}

void Logger::info(const std::string& message) {
    std::cout << formatMessage("INFO", message) << std::endl;
}

void Logger::warn(const std::string& message) {
    std::cout << formatMessage("WARN", message) << std::endl;
}

void Logger::error(const std::string& message) {
    std::cerr << formatMessage("ERROR", message) << std::endl;
}

void Logger::debug(const std::string& message) {
    std::cout << formatMessage("DEBUG", message) << std::endl;
}

std::string Logger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string Logger::formatMessage(const std::string& level, const std::string& message) {
    std::stringstream ss;
    ss << "[" << getCurrentTimestamp() << "] [" << level << "] " << message;
    return ss.str();
}

} // namespace user_service::util 