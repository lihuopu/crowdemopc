#include "user_controller.h"
#include "../service/user_service.h"
#include "../util/logger.h"
#include <iostream>
#include <chrono>

namespace user_service::controller {

grpc::Status UserController::GetUserInfo(grpc::ServerContext* context, 
                                         const user::GetUserInfoRequest* request,
                                         user::GetUserInfoResponse* response) {
    LOG_INFO("收到获取用户信息请求，用户ID: " + std::to_string(request->user_id()));
    
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