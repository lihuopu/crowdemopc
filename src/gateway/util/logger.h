#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <memory>
#include <sstream>
#include <chrono>
#include <iomanip>

// 前向声明
namespace gateway::util {
    struct LogConfig;
}

namespace gateway::util {

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3,
    FATAL = 4
};

class Logger {
public:
    static Logger& getInstance();
    
    // 初始化日志系统
    void init(const std::string& log_dir = "./logs", 
              LogLevel level = LogLevel::INFO,
              size_t max_file_size = 10 * 1024 * 1024,  // 10MB
              size_t max_files = 5);
    
    // 从配置初始化日志系统
    void initFromConfig(const LogConfig& config);
    
    // 日志记录方法
    void debug(const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
    void fatal(const std::string& message);
    
    // 设置日志级别
    void setLogLevel(LogLevel level);
    
    // 关闭日志系统
    void shutdown();

private:
    Logger() = default;
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // 内部方法
    void log(LogLevel level, const std::string& message);
    std::string getCurrentTimestamp();
    std::string getLogLevelString(LogLevel level);
    void rotateLogFile();
    void ensureLogDirectory();
    void checkAndRotateByDate();
    std::string getCurrentDateString();
    
    // 成员变量
    std::string log_dir_;
    std::string current_log_file_;
    std::string current_date_;
    std::string file_prefix_;
    std::ofstream log_file_;
    LogLevel current_level_;
    size_t max_file_size_;
    size_t max_files_;
    std::mutex log_mutex_;
    bool initialized_;
};

// 便捷宏定义
#define LOG_DEBUG(msg) gateway::util::Logger::getInstance().debug(msg)
#define LOG_INFO(msg) gateway::util::Logger::getInstance().info(msg)
#define LOG_WARN(msg) gateway::util::Logger::getInstance().warn(msg)
#define LOG_ERROR(msg) gateway::util::Logger::getInstance().error(msg)
#define LOG_FATAL(msg) gateway::util::Logger::getInstance().fatal(msg)

} // namespace gateway::util
