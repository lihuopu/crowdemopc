#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/async.h>
#include <fmt/format.h>

// 日志器管理类
class LoggerManager {
private:
    std::shared_ptr<spdlog::logger> logger_;
    bool is_async_;
    
    // 私有构造函数，单例模式
    LoggerManager() : is_async_(false) {
        // 默认使用同步模式初始化
        initSyncLogger();
    }

public:
    // 获取单例实例
    static LoggerManager& getInstance() {
        static LoggerManager instance;
        return instance;
    }
    
    // 禁止拷贝和移动
    LoggerManager(const LoggerManager&) = delete;
    LoggerManager& operator=(const LoggerManager&) = delete;
    LoggerManager(LoggerManager&&) = delete;
    LoggerManager& operator=(LoggerManager&&) = delete;
    
    // 初始化同步日志器
    void initSyncLogger() {
        // 创建控制台和文件双 sink
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("app.log", true);
        
        // 设置不同 sink 的日志级别
        console_sink->set_level(spdlog::level::debug);
        file_sink->set_level(spdlog::level::trace);
        
        // 自定义格式
        console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
        file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%#] %v");
        
        // 创建日志器
        logger_ = std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list({console_sink, file_sink}));
        logger_->set_level(spdlog::level::trace);
        
        spdlog::register_logger(logger_);
        is_async_ = false;
        
        logger_->info("同步日志器初始化完成");
    }
    
    // 初始化异步日志器
    void initAsyncLogger() {
        // 异步日志需要先设置线程池
        spdlog::init_thread_pool(8192, 1); // 队列大小 8192，1 个后台线程
        
        // 创建控制台和文件双 sink
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("app_async.log", true);
        
        // 设置不同 sink 的日志级别
        console_sink->set_level(spdlog::level::debug);
        file_sink->set_level(spdlog::level::trace);
        
        // 自定义格式
        console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
        file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%#] %v");
        
        // 创建异步日志器
        logger_ = spdlog::basic_logger_mt<spdlog::async_factory>("async_multi_sink", "app_async.log");
        logger_->sinks().push_back(console_sink);
        logger_->set_level(spdlog::level::trace);
        
        spdlog::register_logger(logger_);
        is_async_ = true;
        
        logger_->info("异步日志器初始化完成");
    }
    
    // 获取当前日志器
    std::shared_ptr<spdlog::logger>& getLogger() {
        return logger_;
    }
    
    // 切换日志模式
    void switchMode(bool async) {
        if (async && !is_async_) {
            initAsyncLogger();
        } else if (!async && is_async_) {
            initSyncLogger();
        }
    }
    
    // 检查当前是否为异步模式
    bool isAsync() const {
        return is_async_;
    }
};

// 日志便捷宏定义
#define LOG_TRACE(...)  LoggerManager::getInstance().getLogger()->trace(__VA_ARGS__)
#define LOG_DEBUG(...)  LoggerManager::getInstance().getLogger()->debug(__VA_ARGS__)
#define LOG_INFO(...)   LoggerManager::getInstance().getLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)   LoggerManager::getInstance().getLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)  LoggerManager::getInstance().getLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...)  LoggerManager::getInstance().getLogger()->critical(__VA_ARGS__)

// 展示各种格式化输出示例
void demonstrateFormatting() {
    // 基本类型格式化
    int int_val = 42;
    float float_val = 3.14159f;
    double double_val = 2.718281828459045;
    const char* cstr_val = "C-style string";
    std::string str_val = "C++ string";
    
    LOG_INFO("基本类型示例:");
    LOG_INFO("整数: {}", int_val);
    LOG_INFO("浮点数: {:.2f}", float_val);
    LOG_INFO("双精度数: {:.6f}", double_val);
    LOG_INFO("C字符串: {}", cstr_val);
    LOG_INFO("C++字符串: {}", str_val);
    
    // 格式化选项
    LOG_INFO("\n格式化选项示例:");
    LOG_INFO("右对齐: [{:>10}]", "test");
    LOG_INFO("左对齐: [{:<10}]", "test");
    LOG_INFO("居中对齐: [{:^10}]", "test");
    LOG_INFO("填充: [{:*^10}]", "test");
    LOG_INFO("十六进制: 0x{:X}", 255);
    LOG_INFO("二进制: {:b}", 42);
    LOG_INFO("百分比: {:.1%}", 0.75);
    
    // 容器格式化
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::vector<std::string> str_vec = {"apple", "banana", "cherry"};
    
    // LOG_INFO("\n容器示例:");
    // LOG_INFO("整数向量: {}", fmt::join(vec, ", "));
    // LOG_INFO("字符串向量: {}", fmt::join(str_vec, ", "));
    
    // 时间格式化
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    
    LOG_INFO("\n时间示例:");
    LOG_INFO("当前时间: {}", std::ctime(&now_time));
    
    // 自定义对象格式化 - 需要为自定义类型实现operator<<
    struct Person {
        std::string name;
        int age;
    };
    
    Person person = {"John Doe", 30};
    LOG_INFO("\n自定义对象示例:");
    LOG_INFO("Person: {} (Age: {})", person.name, person.age);
}

int main() {
    try {
        // 获取日志管理器实例
        auto& loggerManager = LoggerManager::getInstance();
        
        // 同步模式下输出日志
        LOG_INFO("===== 同步模式日志示例 =====");
        LOG_TRACE("这是一条TRACE级别的日志");
        LOG_DEBUG("这是一条DEBUG级别的日志");
        LOG_INFO("这是一条INFO级别的日志");
        LOG_WARN("这是一条WARN级别的日志");
        LOG_ERROR("这是一条ERROR级别的日志");
        LOG_CRITICAL("这是一条CRITICAL级别的日志");
        
        // 展示各种格式化输出
        LOG_INFO("\n===== 格式化输出示例 =====");
        demonstrateFormatting();
        
        // 切换到异步模式
        loggerManager.switchMode(true);
        LOG_INFO("\n===== 异步模式日志示例 =====");
        
        // 异步模式下输出日志
        LOG_TRACE("这是一条异步TRACE级别的日志");
        LOG_DEBUG("这是一条异步DEBUG级别的日志");
        LOG_INFO("这是一条异步INFO级别的日志");
        LOG_WARN("这是一条异步WARN级别的日志");
        LOG_ERROR("这是一条异步ERROR级别的日志");
        LOG_CRITICAL("这是一条异步CRITICAL级别的日志");
        
        // 确保所有异步日志都被处理
        spdlog::shutdown();
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "日志初始化失败: " << ex.what() << std::endl;
        return 1;
    }
    
    return 0;
}
