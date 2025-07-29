#include "grpc_service_impl.h"
#include "../controller/user_controller.h"

namespace user_service::service {

grpc::Status GrpcServiceImpl::GetUserInfo(grpc::ServerContext* context, 
                                          const user::GetUserInfoRequest* request,
                                          user::GetUserInfoResponse* response) {
    // 委托给控制器处理
    return user_service::controller::UserController::GetUserInfo(context, request, response);
}

} // namespace user_service::service 