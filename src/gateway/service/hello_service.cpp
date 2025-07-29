#include "hello_service.h"
#include "../util/response_util.h"
#include "../util/logger.h"
#include <chrono>
#include <regex>

namespace gateway::service {

HelloService& HelloService::getInstance() {
    static HelloService instance;
    return instance;
}

crow::json::wvalue HelloService::generateGreeting(const std::string& request_id) {
    LOG_INFO("RequestID: " + request_id + " | Generating greeting");
    
    auto response = gateway::util::success_response("Hello from Gateway Service!");
    response["request_id"] = request_id;
    response["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    LOG_INFO("RequestID: " + request_id + " | Greeting generated successfully");
    return response;
}

crow::json::wvalue HelloService::generatePersonalizedGreeting(const std::string& request_id, const std::string& name) {
    LOG_INFO("RequestID: " + request_id + " | Generating personalized greeting for name: " + name);
    
    // 验证用户名
    if (!validateName(name)) {
        auto response = gateway::util::error_response("Invalid name format", 400);
        response["request_id"] = request_id;
        response["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        return response;
    }
    
    crow::json::wvalue response;
    response["success"] = true;
    response["message"] = "Greeting generated successfully";
    response["data"]["greeting"] = formatGreeting(name);
    response["data"]["name"] = name;
    response["request_id"] = request_id;
    response["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    LOG_INFO("RequestID: " + request_id + " | Personalized greeting generated successfully");
    return response;
}

bool HelloService::validateName(const std::string& name) {
    // 检查名称是否为空或过长
    if (name.empty() || name.length() > 50) {
        return false;
    }
    
    // 检查名称是否只包含字母、数字、空格和连字符
    std::regex name_pattern("^[a-zA-Z0-9\\s\\-]+$");
    return std::regex_match(name, name_pattern);
}

std::string HelloService::formatGreeting(const std::string& name) {
    return "Hello, " + name + "!";
}

} // namespace gateway::service 