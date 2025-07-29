#pragma once

#include <grpcpp/grpcpp.h>
#include "../proto/user.grpc.pb.h"
#include <memory>
#include <string>

namespace user_service::controller {

class UserController {
public:
    // 获取用户信息
    static grpc::Status GetUserInfo(grpc::ServerContext* context, 
                                   const user::GetUserInfoRequest* request,
                                   user::GetUserInfoResponse* response);
    
    // 验证用户ID
    static bool validateUserId(int32_t user_id);
    
    // 检查用户是否存在
    static bool userExists(int32_t user_id);
};

} // namespace user_service::controller 