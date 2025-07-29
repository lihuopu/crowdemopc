#pragma once

#include "crow/json.h"
#include "crow/http_request.h"
#include <string>
#include <memory>

// 前向声明
namespace user_service::service {
    class UserClient;
}

namespace gateway::service {

class ApiService {
public:
    static ApiService& getInstance();
    
    // 处理POST数据
    crow::json::wvalue processPostData(const std::string& request_id, const crow::request& req);
    
    // 获取用户信息
    crow::json::wvalue getUserInfo(const std::string& request_id, int user_id);
    
    // 验证用户ID
    bool validateUserId(int user_id);
    
    // 解析JSON数据
    crow::json::wvalue parseJsonData(const std::string& json_string);

private:
    ApiService();
    ~ApiService() = default;
    ApiService(const ApiService&) = delete;
    ApiService& operator=(const ApiService&) = delete;
    
    // 生成模拟用户数据（备用方案）
    crow::json::wvalue generateMockUserData(int user_id);
    
    // 验证JSON格式
    bool validateJsonFormat(const std::string& json_string);
    
    // 将 gRPC 用户信息转换为 JSON
    crow::json::wvalue convertUserInfoToJson(const void* user_info_response);
    
    // 用户服务客户端
    std::unique_ptr<user_service::service::UserClient> user_client_;
    bool user_service_available_;
};

} // namespace gateway::service 