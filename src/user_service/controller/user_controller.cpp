#include "user_controller.h"
#include "../service/user_service.h"
#include "../../../testredis/redis.hpp"
#include "../util/logger.h"
#include <iostream>
#include <sstream>
#include <chrono>

namespace user_service::controller {

grpc::Status UserController::GetUserInfo(grpc::ServerContext* context, 
                                         const user::GetUserInfoRequest* request,
                                         user::GetUserInfoResponse* response) {
    LOG_INFO("收到获取用户信息请求，用户ID: " + std::to_string(request->user_id()));
    
    
    // 定义Redis发布通道（可自定义，需与redis_client订阅的通道一致）
    const int REDIS_CHANNEL = 200;  // 避免与现有通道冲突

    // 创建Redis实例并连接
    Redis redis;
    if (!redis.connect()) {
        LOG_WARN("Redis连接失败，无法发布消息");
    }

    // 构造请求信息字符串（JSON格式便于解析）
    std::stringstream req_ss;
    req_ss << "{"
        << "\"type\":\"request\","
        << "\"user_id\":" << request->user_id() << ","
        << "\"timestamp\":" << std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count()
        << "}";
    std::string req_msg = req_ss.str();

    // 发布请求信息到Redis通道（修正日志输出）
    if (redis.connect()) {
        redis.publish(REDIS_CHANNEL, req_msg);
        // 使用stringstream拼接日志内容
        std::stringstream log_ss;
        log_ss << "已发布请求信息到Redis通道 " << REDIS_CHANNEL << ": " << req_msg;
        LOG_INFO(log_ss.str());  // 传入拼接后的字符串
    }
    
    // 验证用户ID
    if (!validateUserId(request->user_id())) {
        response->set_success(false);
        response->set_message("无效的用户ID");
        response->set_status_code(400);
        response->set_timestamp(std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
        return grpc::Status::OK;
    }
    
    // 检查用户是否存在
    if (!userExists(request->user_id())) {
        response->set_success(false);
        response->set_message("用户不存在");
        response->set_status_code(404);
        response->set_timestamp(std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
        return grpc::Status::OK;
    }
    
    try {
        // 调用服务层获取用户数据
        auto user_info = user_service::service::UserService::getInstance().getUserInfo(request->user_id());
        
        response->set_success(true);
        response->set_message("用户信息获取成功");
        response->set_status_code(200);
        response->mutable_user_info()->CopyFrom(user_info);
        response->set_timestamp(std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
        
        LOG_INFO("用户信息获取成功，用户ID: " + std::to_string(request->user_id()));
        
    } catch (const std::exception& e) {
        response->set_success(false);
        response->set_message("获取用户信息失败: " + std::string(e.what()));
        response->set_status_code(500);
        response->set_timestamp(std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
        
        LOG_ERROR("获取用户信息失败，用户ID: " + std::to_string(request->user_id()) 
                  + "，错误: " + e.what());
    }
    
    return grpc::Status::OK;
}

bool UserController::validateUserId(int32_t user_id) {
    return user_id > 0 && user_id <= 1000000; // 假设最大用户ID为100万
}

bool UserController::userExists(int32_t user_id) {
    // 模拟数据库查询，这里简单判断用户ID是否在有效范围内
    // 在实际应用中，这里应该查询真实的数据库
    return user_id > 0 && user_id <= 100000;
}

} // namespace user_service::controller 