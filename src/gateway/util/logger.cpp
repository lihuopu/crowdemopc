#include "logger.h"
#include "config.h"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <sys/stat.h>

namespace gateway::util {

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::init(const std::string& log_dir, LogLevel level, size_t max_file_size, size_t max_files) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    
    if (initialized_) {
        return;
    }
    
    log_dir_ = log_dir;
    current_level_ = level;
    max_file_size_ = max_file_size;
    max_files_ = max_files;
    file_prefix_ = "gateway";  // 默认前缀
    
    std::cout << "Initializing logger with directory: " << log_dir_ << std::endl;
    
    ensureLogDirectory();
    
    // 创建当前日志文件，按日期命名
    current_date_ = getCurrentDateString();
    current_log_file_ = log_dir_ + "/" + file_prefix_ + "_" + current_date_ + ".log";
    
    std::cout << "Creating log file: " << current_log_file_ << std::endl;
    
    log_file_.open(current_log_file_, std::ios::app);
    if (!log_file_.is_open()) {
        std::cerr << "Failed to open log file: " << current_log_file_ << std::endl;
        return;
    }
    
    initialized_ = true;
    std::cout << "Logger initialized successfully" << std::endl;
    
    // 直接写入一条初始化日志
    std::string init_msg = "Logger initialized. Log file: " + current_log_file_;
    log_file_ << getCurrentTimestamp() << " [INFO ] " << init_msg << std::endl;
    log_file_.flush();
}

void Logger::initFromConfig(const LogConfig& config) {
    // 将字符串日志级别转换为枚举
    LogLevel level = LogLevel::INFO;  // 默认值
    if (config.level == "DEBUG") level = LogLevel::DEBUG;
    else if (config.level == "INFO") level = LogLevel::INFO;
    else if (config.level == "WARN") level = LogLevel::WARN;
    else if (config.level == "ERROR") level = LogLevel::ERROR;
    else if (config.level == "FATAL") level = LogLevel::FATAL;
    
    init(config.directory, level, config.max_file_size, config.max_files);
    
    // 设置文件前缀
    file_prefix_ = config.file_prefix;
    
    // 重新创建日志文件（使用新的前缀）
    if (log_file_.is_open()) {
        log_file_.close();
    }
    
    current_date_ = getCurrentDateString();
    current_log_file_ = log_dir_ + "/" + file_prefix_ + "_" + current_date_ + ".log";
    
    log_file_.open(current_log_file_, std::ios::app);
    if (!log_file_.is_open()) {
        std::cerr << "Failed to open log file: " << current_log_file_ << std::endl;
        return;
    }
    
    // 写入配置初始化日志
    std::string config_msg = "Logger initialized from config. Log file: " + current_log_file_;
    log_file_ << getCurrentTimestamp() << " [INFO ] " << config_msg << std::endl;
    log_file_.flush();
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warn(const std::string& message) {
    log(LogLevel::WARN, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::fatal(const std::string& message) {
    log(LogLevel::FATAL, message);
}

void Logger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    current_level_ = level;
}

void Logger::shutdown() {
    std::lock_guard<std::mutex> lock(log_mutex_);
    if (log_file_.is_open()) {
        std::string shutdown_msg = "Logger shutting down";
        log_file_ << getCurrentTimestamp() << " [INFO ] " << shutdown_msg << std::endl;
        log_file_.close();
    }
    initialized_ = false;
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < current_level_ || !initialized_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(log_mutex_);
    
    // 检查日期是否变化，如果变化则轮转日志文件
    checkAndRotateByDate();
    
    // 检查文件大小，如果超过限制则轮转
    if (log_file_.tellp() > static_cast<std::streampos>(max_file_size_)) {
        rotateLogFile();
    }
    
    std::string timestamp = getCurrentTimestamp();
    std::string level_str = getLogLevelString(level);
    
    std::string log_entry = timestamp + " [" + level_str + "] " + message + "\n";
    
    log_file_ << log_entry;
    log_file_.flush();  // 确保立即写入磁盘
}

std::string Logger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string Logger::getLogLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO ";
        case LogLevel::WARN:  return "WARN ";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

void Logger::rotateLogFile() {
    if (log_file_.is_open()) {
        log_file_.close();
    }
    
    // 清理旧日志文件
    std::vector<std::string> log_files;
    for (const auto& entry : std::filesystem::directory_iterator(log_dir_)) {
        if (entry.is_regular_file() && entry.path().extension() == ".log") {
            log_files.push_back(entry.path().string());
        }
    }
    
    // 按修改时间排序
    std::sort(log_files.begin(), log_files.end(), [](const std::string& a, const std::string& b) {
        struct stat stat_a, stat_b;
        stat(a.c_str(), &stat_a);
        stat(b.c_str(), &stat_b);
        return stat_a.st_mtime > stat_b.st_mtime;
    });
    
    // 删除多余的日志文件
    while (log_files.size() >= max_files_) {
        std::filesystem::remove(log_files.back());
        log_files.pop_back();
    }
    
    // 创建新的日志文件，按日期命名
    current_date_ = getCurrentDateString();
    current_log_file_ = log_dir_ + "/" + file_prefix_ + "_" + current_date_ + ".log";
    
    log_file_.open(current_log_file_, std::ios::app);
    if (!log_file_.is_open()) {
        std::cerr << "Failed to open new log file: " << current_log_file_ << std::endl;
    }
}

void Logger::ensureLogDirectory() {
    std::cout << "Ensuring log directory exists: " << log_dir_ << std::endl;
    if (!std::filesystem::exists(log_dir_)) {
        std::cout << "Creating log directory: " << log_dir_ << std::endl;
        std::filesystem::create_directories(log_dir_);
        std::cout << "Log directory created successfully" << std::endl;
    } else {
        std::cout << "Log directory already exists" << std::endl;
    }
}

void Logger::checkAndRotateByDate() {
    std::string current_date = getCurrentDateString();
    if (current_date != current_date_) {
        // 日期变化，需要轮转日志文件
        if (log_file_.is_open()) {
            log_file_.close();
        }
        
        current_date_ = current_date;
        current_log_file_ = log_dir_ + "/" + file_prefix_ + "_" + current_date_ + ".log";
        
        log_file_.open(current_log_file_, std::ios::app);
        if (!log_file_.is_open()) {
            std::cerr << "Failed to open new log file: " << current_log_file_ << std::endl;
        } else {
            // 写入新日志文件的开始标记
            std::string start_msg = "New log file started for date: " + current_date_;
            log_file_ << getCurrentTimestamp() << " [INFO ] " << start_msg << std::endl;
            log_file_.flush();
        }
    }
}

std::string Logger::getCurrentDateString() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y%m%d");
    return ss.str();
}

Logger::~Logger() {
    shutdown();
}

} // namespace gateway::util 