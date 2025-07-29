#pragma once

#include "crow/json.h"
#include <string>

namespace gateway::service {

class HelloService {
public:
    static HelloService& getInstance();
    
    // 生成问候语
    crow::json::wvalue generateGreeting(const std::string& request_id);
    
    // 生成个性化问候语
    crow::json::wvalue generatePersonalizedGreeting(const std::string& request_id, const std::string& name);
    
    // 验证用户名
    bool validateName(const std::string& name);

private:
    HelloService() = default;
    ~HelloService() = default;
    HelloService(const HelloService&) = delete;
    HelloService& operator=(const HelloService&) = delete;
    
    // 格式化问候语
    std::string formatGreeting(const std::string& name);
};

} // namespace gateway::service 