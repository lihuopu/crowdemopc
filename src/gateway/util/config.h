#pragma once

#include <string>
#include <yaml-cpp/yaml.h>

namespace gateway::util {

struct LogConfig {
    std::string directory;
    std::string level;
    size_t max_file_size;
    size_t max_files;
    std::string file_prefix;
};

struct ServerConfig {
    int port;
    std::string host;
    int threads;
    LogConfig log;
};

class ConfigManager {
public:
    static ConfigManager& getInstance();
    
    // 加载配置文件
    bool loadConfig(const std::string& config_file);
    
    // 获取服务器配置
    const ServerConfig& getServerConfig() const { return server_config_; }
    
    // 获取日志配置
    const LogConfig& getLogConfig() const { return server_config_.log; }

private:
    ConfigManager() = default;
    ~ConfigManager() = default;
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    
    // 解析日志配置
    LogConfig parseLogConfig(const YAML::Node& log_node);
    
    ServerConfig server_config_;
    bool loaded_ = false;
};

} // namespace gateway::util 