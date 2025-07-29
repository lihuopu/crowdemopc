#pragma once

#include <grpcpp/grpcpp.h>
#include "../proto/user.grpc.pb.h"
#include <memory>
#include <string>

namespace user_service::service {

class GrpcServiceImpl final : public user::UserService::Service {
public:
    // 获取用户信息
    grpc::Status GetUserInfo(grpc::ServerContext* context, 
                            const user::GetUserInfoRequest* request,
                            user::GetUserInfoResponse* response) override;
};

} // namespace user_service::service 