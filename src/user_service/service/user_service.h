#pragma once

#include "../proto/user.pb.h"
#include <memory>
#include <string>

namespace user_service::service {

class UserService {
public:
    static UserService& getInstance();
    
    // 获取用户信息
    user::UserInfo getUserInfo(int32_t user_id);
    
    // 生成用户数据（模拟数据库查询）
    user::UserInfo generateUserData(int32_t user_id);

private:
    UserService() = default;
    ~UserService() = default;
    UserService(const UserService&) = delete;
    UserService& operator=(const UserService&) = delete;
};

} // namespace user_service::service 