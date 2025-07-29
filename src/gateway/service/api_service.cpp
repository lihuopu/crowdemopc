#include "api_service.h"
#include "../util/logger.h"
#include "../../user_service/service/user_client.h"
#include <chrono>
#include <exception>

namespace gateway::service {

ApiService::ApiService() : user_service_available_(false) {

    try {
        // 尝试连接用户服务
        user_client_ = std::make_unique<user_service::service::UserClient>("localhost:50051");
        if (user_client_->IsConnected()) {
            user_service_available_ = true;
            
            LOG_INFO("用户服务连接成功");
        } else {
            LOG_WARN("用户服务连接失败，将使用模拟数据");
        }
    } catch (const std::exception& e) {
        LOG_ERROR("初始化用户服务客户端失败: " + std::string(e.what()));
        user_service_available_ = false;
    }
}

ApiService& ApiService::getInstance() {
    static ApiService instance;
    return instance;
}

crow::json::wvalue ApiService::processPostData(const std::string& request_id, const crow::request& req) {
    LOG_INFO("RequestID: " + request_id + " | Processing POST data request");

    crow::json::wvalue response;
    response["success"] = true;
    response["message"] = "POST请求处理成功";
    response["request_id"] = request_id;
    
    try {
        // 验证JSON格式
        if (!validateJsonFormat(req.body)) {
            response["success"] = false;
            response["error"] = "无效的JSON格式";
            response["status_code"] = 400;
        } else {
            // 解析JSON数据
            auto parsed_data = parseJsonData(req.body);
            response["data"]["received_data"] = std::move(parsed_data);
            response["data"]["message"] = "数据接收成功";
            
            LOG_INFO("RequestID: " + request_id + " | POST data processed successfully");
        }
    } catch (const std::exception& e) {
        response["success"] = false;
        response["error"] = "服务器内部错误: " + std::string(e.what());
        response["status_code"] = 500;
        LOG_ERROR("RequestID: " + request_id + " | POST data processing error: " + e.what());
    }
    
    response["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    return response;
}

crow::json::wvalue ApiService::getUserInfo(const std::string& request_id, int user_id) {
    LOG_INFO("RequestID: " + request_id + " | Processing user info request for user_id: " + std::to_string(user_id));
    
    crow::json::wvalue response;
    response["request_id"] = request_id;
    
    // 验证用户ID
    if (!validateUserId(user_id)) {
        response["success"] = false;
        response["error"] = "无效的用户ID";
        response["status_code"] = 400;
    } else {
        try {
            if (user_service_available_ && user_client_) {
                // 调用用户服务
                auto [success, grpc_response] = user_client_->GetUserInfo(user_id);
                
                if (success && grpc_response.success()) {
                    // 转换 gRPC 响应为 JSON
                    auto user_info = grpc_response.user_info();
                    crow::json::wvalue user_data;
                    user_data["user_id"] = user_info.user_id();
                    user_data["username"] = user_info.username();
                    user_data["email"] = user_info.email();
                    user_data["status"] = user_info.status();
                    user_data["created_at"] = user_info.created_at();
                    user_data["last_login"] = user_info.last_login();
                    user_data["full_name"] = user_info.full_name();
                    user_data["phone"] = user_info.phone();
                    user_data["avatar_url"] = user_info.avatar_url();
                    
                    response["success"] = true;
                    response["message"] = "用户信息获取成功（来自用户服务）";
                    response["data"] = std::move(user_data);
                    
                    LOG_INFO("RequestID: " + request_id + " | User info retrieved from user service successfully");
                } else {
                    // gRPC 调用失败，使用模拟数据
                    LOG_WARN("RequestID: " + request_id + " | User service call failed, using mock data");
                    auto user_data = generateMockUserData(user_id);
                    response["success"] = true;
                    response["message"] = "用户信息获取成功（模拟数据）";
                    response["data"] = std::move(user_data);
                }
            } else {
                // 用户服务不可用，使用模拟数据
                LOG_WARN("RequestID: " + request_id + " | User service not available, using mock data");
                auto user_data = generateMockUserData(user_id);
                response["success"] = true;
                response["message"] = "用户信息获取成功（模拟数据）";
                response["data"] = std::move(user_data);
            }
            
        } catch (const std::exception& e) {
            response["success"] = false;
            response["error"] = "获取用户信息失败: " + std::string(e.what());
            response["status_code"] = 500;
            LOG_ERROR("RequestID: " + request_id + " | User info retrieval error: " + e.what());
        }
    }
    
    response["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    return response;
}

bool ApiService::validateUserId(int user_id) {
    return user_id > 0 && user_id <= 1000000; // 假设最大用户ID为100万
}

crow::json::wvalue ApiService::parseJsonData(const std::string& json_string) {
    auto body = crow::json::load(json_string);
    if (!body) {
        throw std::runtime_error("Failed to parse JSON");
    }
    return body;
}

crow::json::wvalue ApiService::generateMockUserData(int user_id) {
    crow::json::wvalue user_data;
    user_data["user_id"] = user_id;
    user_data["username"] = "user_" + std::to_string(user_id);
    user_data["email"] = "user" + std::to_string(user_id) + "@example.com";
    user_data["status"] = "active";
    user_data["created_at"] = "2024-01-01T00:00:00Z";
    user_data["last_login"] = "2024-12-01T12:00:00Z";
    
    return user_data;
}

bool ApiService::validateJsonFormat(const std::string& json_string) {
    if (json_string.empty()) {
        return false;
    }
    
    try {
        auto body = crow::json::load(json_string);
        return body != nullptr;
    } catch (...) {
        return false;
    }
}

} // namespace gateway::service 