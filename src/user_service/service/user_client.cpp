#include "user_client.h"
#include "../util/logger.h"
#include <iostream>
#include <grpcpp/grpcpp.h>

namespace user_service::service {

UserClient::UserClient(const std::string& server_address) 
    : server_address_(server_address), is_connected_(false) {
    
    auto channel = createChannel(server_address);
    if (channel) {
        stub_ = user::UserService::NewStub(channel);
        is_connected_ = true;
        LOG_INFO("用户服务客户端连接成功，服务器地址: " + server_address);
    } else {
        LOG_ERROR("用户服务客户端连接失败，服务器地址: " + server_address);
    }
}

std::shared_ptr<grpc::Channel> UserClient::createChannel(const std::string& server_address) {
    try {
        return grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
    } catch (const std::exception& e) {
        LOG_ERROR("创建 gRPC 通道失败: " + std::string(e.what()));
        return nullptr;
    }
}

std::pair<bool, user::GetUserInfoResponse> UserClient::GetUserInfo(int32_t user_id) {
    if (!is_connected_ || !stub_) {
        user::GetUserInfoResponse response;
        response.set_success(false);
        response.set_message("客户端未连接");
        response.set_status_code(500);
        return {false, response};
    }
    
    user::GetUserInfoRequest request;
    request.set_user_id(user_id);
    
    grpc::ClientContext context;
    user::GetUserInfoResponse response;
    
    // 设置超时时间
    std::chrono::system_clock::time_point deadline = 
        std::chrono::system_clock::now() + std::chrono::seconds(5);
    context.set_deadline(deadline);
    
    grpc::Status status = stub_->GetUserInfo(&context, request, &response);
    
    if (!status.ok()) {
        LOG_ERROR("gRPC 调用失败: " + status.error_message());
        user::GetUserInfoResponse error_response;
        error_response.set_success(false);
        error_response.set_message("gRPC 调用失败: " + status.error_message());
        error_response.set_status_code(500);
        return {false, error_response};
    }
    
    return {true, response};
}

bool UserClient::IsConnected() const {
    return is_connected_;
}

} // namespace user_service::service 