#include "api_service.h"
#include "../util/logger.h"
#include "../../user_service/service/user_client.h"
#include <chrono>
#include <exception>

namespace gateway::service {

// ApiService::ApiService() : user_service_available_(false) {

//     try {
//         // 尝试连接用户服务
//         user_client_ = std::make_unique<user_service::service::UserClient>("localhost:50051");
//         if (user_client_->IsConnected()) {
//             user_service_available_ = true;
            
//             LOG_INFO("用户服务连接成功");
//         } else {
//             LOG_WARN("用户服务连接失败，将使用模拟数据");
//         }
//     } catch (const std::exception& e) {
//         LOG_ERROR("初始化用户服务客户端失败: " + std::string(e.what()));
//         user_service_available_ = false;
//     }
// }

// 服务上线回调
void on_service_added(const std::string& key, const std::string& value) {
    std::cout << "检测到服务上线: " << key << " -> " << value << std::endl;
}

// 服务下线回调
void on_service_removed(const std::string& key, const std::string& value) {
    std::cout << "检测到服务下线: " << key << std::endl;
}

ApiService::ApiService() : user_service_available_(false) {
    std::cout << ">>>>> ApiService构造函数开始执行 <<<<<" << std::endl; 
    // 构造函数只做基础初始化，不执行网络操作
    LOG_INFO("ApiService 初始化开始");
    
    // 用后台线程初始化服务发现，避免阻塞
    std::thread discovery_thread([this]() {
        try {
            std::cout << ">>>>> 后台线程：开始初始化Discovery <<<<<" << std::endl;
            auto discovery_ = std::make_shared<chat_server::Discovery>(
                "http://localhost:2379",
                "user_service",
                // on_service_added, 
                // on_service_removed
                std::bind(&ApiService::onServiceAdded, this, std::placeholders::_1, std::placeholders::_2),
                std::bind(&ApiService::onServiceRemoved, this, std::placeholders::_1, std::placeholders::_2)
            );

            // 加载服务列表（移到后台线程执行）
            std::cout << ">>>>> 后台线程：开始获取服务列表 <<<<<" << std::endl;
            auto initial_services = discovery_->get_services();
            std::cout << ">>>>> 后台线程：get_services()返回，服务数量=" << initial_services.size() << std::endl;
            {
                std::lock_guard<std::mutex> lock(services_mutex_);
                available_services_ = std::move(initial_services);
                user_service_available_ = !available_services_.empty();
                
                if (user_service_available_) {
                    refreshUserClient();
                    LOG_INFO("服务发现初始化成功，可用服务数: " + std::to_string(available_services_.size()));
                    discovery_succeeded_ = true;  // 标记成功
                } else {
                    
                    LOG_WARN("未找到可用的用户服务");
                    discovery_succeeded_ = false;
                }
            }
        } catch (const std::exception& e) {
            std::cout << ">>>>> 后台线程异常: " << e.what() << std::endl; 
            LOG_ERROR("服务发现初始化失败: " + std::string(e.what()));
            discovery_succeeded_ = false;
        }
        // 【关键修正】在后台线程执行完毕后，再标记完成
        discovery_finished_ = true;
        discovery_cv_.notify_all();
    });
    
    // 分离线程，避免阻塞构造函数
    discovery_thread.detach();
}

void ApiService::onServiceAdded(const std::string& key, const std::string& value) {
    std::cout << ">>>>> 成员函数：服务上线事件触发，key=" << key << "，value=" << value << std::endl;  // 新增
    LOG_INFO("收到服务上线事件: " + key);  // 先输出日志，确认回调被执行
    //std::lock_guard<std::mutex> lock(services_mutex_);
    // 避免重复添加相同服务
    auto it = std::find_if(available_services_.begin(), available_services_.end(),
        [&](const std::pair<std::string, std::string>& pair) {
            return pair.first == key;
        });
    if (it == available_services_.end()) {
        available_services_.emplace_back(key, value);
        user_service_available_ = true;
        LOG_INFO("用户服务上线: " + key + " -> " + value);
        refreshUserClient();  // 刷新连接到新服务
    }
}


void ApiService::onServiceRemoved(const std::string& key, const std::string& value) {
    std::cout << ">>>>> 成员函数：服务下线事件触发，key=" << key << std::endl;  // 新增
    //std::lock_guard<std::mutex> lock(services_mutex_);
    // 移除下线的服务
    auto it = std::remove_if(available_services_.begin(), available_services_.end(),
        [&](const std::pair<std::string, std::string>& pair) {
            return pair.first == key;
        });
    if (it != available_services_.end()) {
        available_services_.erase(it, available_services_.end());
        LOG_INFO("用户服务下线: " + key);
        
        // 检查是否还有可用服务
        if (available_services_.empty()) {
            user_service_available_ = false;
            user_client_.reset();  // 释放无效客户端
        } else {
            refreshUserClient();  // 切换到其他可用服务
        }
    }
}

void ApiService::refreshUserClient() {
    cout << "客户端更新" <<endl;
    //std::lock_guard<std::mutex> lock(services_mutex_);
    cout << "客户端更新：获取锁成功" << endl;  // 步骤2
    if (available_services_.empty()) {
        // 无可用服务，重置客户端
        user_client_.reset();
        user_service_available_ = false;
        return;
    }

    // 从可用服务列表中选一个（当前是第一个，可扩展为负载均衡策略）
    std::string service_info = available_services_[0].second;  // value是JSON字符串
    cout << "客户端更新：服务信息=" << service_info << endl;
    // 解析JSON，提取IP（host）和端口（port）
    auto json_data = crow::json::load(service_info);
    std::string host = json_data["host"].s();  // 后端服务IP
    int port = json_data["port"].i();          // 后端服务端口
    std::string server_address = host + ":" + std::to_string(port);  // 拼接为"IP:端口"

    // 创建gRPC客户端，连接到解析出的IP和端口
    user_client_ = std::make_unique<user_service::service::UserClient>(server_address);
    user_service_available_ = user_client_->IsConnected();  // 标记服务可用状态
    cout << "客户端更新函数完成" <<endl;
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
    

    // 等待服务发现初始化完成（最多等待3秒，避免无限阻塞）
    {
        std::unique_lock<std::mutex> lock(services_mutex_);
        // 等待条件：discovery_finished_为true，或超时3秒
        bool finished = discovery_cv_.wait_for(lock, std::chrono::seconds(3), 
            [this]() { return discovery_finished_.load() ; });
        
        if (!finished) {
            LOG_WARN("RequestID: " + request_id + " | 服务发现初始化超时，使用模拟数据");
        } else if (!discovery_succeeded_) {
            LOG_WARN("RequestID: " + request_id + " | 服务发现初始化失败，使用模拟数据");
        }
    }

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