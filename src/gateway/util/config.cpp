#include "config.h"
#include "logger.h"
#include <iostream>
#include <filesystem>

namespace gateway::util {

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

bool ConfigManager::loadConfig(const std::string& config_file) {
    try {
        // 检查配置文件是否存在
        if (!std::filesystem::exists(config_file)) {
            std::cerr << "配置文件不存在: " << config_file << std::endl;
            return false;
        }
        
        // 加载YAML文件
        YAML::Node config = YAML::LoadFile(config_file);
        
        // 解析服务器配置
        if (!config["server"]) {
            std::cerr << "配置文件中缺少 'server' 节点" << std::endl;
            return false;
        }
        
        YAML::Node server_node = config["server"];
        
        // 解析端口配置
        server_config_.port = server_node["port"].as<int>();
        server_config_.host = server_node["host"].as<std::string>();
        server_config_.threads = server_node["threads"].as<int>();
        
        // 解析日志配置
        if (server_node["log"]) {
            server_config_.log = parseLogConfig(server_node["log"]);
        } else {
            // 使用默认日志配置
            server_config_.log.directory = "./logs";
            server_config_.log.level = "INFO";
            server_config_.log.max_file_size = 10 * 1024 * 1024; // 10MB
            server_config_.log.max_files = 5;
            server_config_.log.file_prefix = "gateway";
        }
        
        loaded_ = true;
        std::cout << "配置文件加载成功: " << config_file << std::endl;
        return true;
        
    } catch (const YAML::Exception& e) {
        std::cerr << "YAML解析错误: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "配置文件加载错误: " << e.what() << std::endl;
        return false;
    }
}

LogConfig ConfigManager::parseLogConfig(const YAML::Node& log_node) {
    LogConfig log_config;
    
    // 解析日志目录
    log_config.directory = log_node["directory"].as<std::string>();
    
    // 解析日志级别
    log_config.level = log_node["level"].as<std::string>();
    
    // 解析最大文件大小
    log_config.max_file_size = log_node["max_file_size"].as<size_t>();
    
    // 解析保留文件数量
    log_config.max_files = log_node["max_files"].as<size_t>();
    
    // 解析文件前缀
    log_config.file_prefix = log_node["file_prefix"].as<std::string>();
    
    return log_config;
}

} // namespace gateway::util 