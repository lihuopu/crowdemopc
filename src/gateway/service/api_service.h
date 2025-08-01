#pragma once

#include "crow/json.h"
#include "crow/http_request.h"
#include <string>
#include <memory>
#include <mutex>
#include <atomic>
#include <functional>
#include <utility>  // 用于std::pair
#include "../../../testetcd/discovery.h"
#include "../../../testetcd/registry.h"
#include <condition_variable>

// 前向声明
namespace user_service::service {
    class UserClient;
}

namespace chat_server{
    class Discovery;
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

    // 服务上线回调（处理etcd通知）
    void onServiceAdded(const std::string& key, const std::string& value);
    // 服务下线回调（处理etcd通知）
    void onServiceRemoved(const std::string& key, const std::string& value);

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
    

    // 新增：服务发现相关函数
    // 刷新gRPC客户端连接（从可用服务中选择）
    void refreshUserClient();
    

    // 用户服务客户端
    std::unique_ptr<user_service::service::UserClient> user_client_;
    bool user_service_available_;

    // 新增：服务发现相关成员
    std::shared_ptr<chat_server::Discovery> discovery_;  // 服务发现实例
    std::vector<std::pair<std::string, std::string>> available_services_;  // 可用服务列表（键-值对）
    mutable std::mutex services_mutex_;  // 保护服务列表的互斥锁
    std::atomic<bool> is_discovery_running_;  // 

    // ... 原有成员 ...
    std::condition_variable discovery_cv_;  // 用于等待服务发现完成
    std::atomic<bool> discovery_finished_ = false;  // 标记服务发现是否完成
    std::atomic<bool> discovery_succeeded_ = false;  // 标记服务发现是否成功
};

} // namespace gateway::service 