#pragma once

#include <grpcpp/grpcpp.h>
#include "../proto/user.grpc.pb.h"
#include <memory>
#include <string>

namespace user_service::service {

class UserClient {
public:
    UserClient(const std::string& server_address);
    ~UserClient() = default;
    
    // 获取用户信息
    std::pair<bool, user::GetUserInfoResponse> GetUserInfo(int32_t user_id);
    
    // 检查连接状态
    bool IsConnected() const;

private:
    std::unique_ptr<user::UserService::Stub> stub_;
    std::string server_address_;
    bool is_connected_;
    
    // 创建通道
    std::shared_ptr<grpc::Channel> createChannel(const std::string& server_address);
};

} // namespace user_service::service 