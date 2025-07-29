#include "user_service.h"
#include "../util/logger.h"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <cstdlib>

namespace user_service::service {

UserService& UserService::getInstance() {
    static UserService instance;
    return instance;
}

user::UserInfo UserService::getUserInfo(int32_t user_id) {
    LOG_INFO("服务层：获取用户信息，用户ID: " + std::to_string(user_id));
    return generateUserData(user_id);
}

user::UserInfo UserService::generateUserData(int32_t user_id) {
    user::UserInfo user_info;
    
    user_info.set_user_id(user_id);
    user_info.set_username("user_" + std::to_string(user_id));
    user_info.set_email("user" + std::to_string(user_id) + "@example.com");
    user_info.set_status("active");
    user_info.set_full_name("用户 " + std::to_string(user_id));
    user_info.set_phone("+86 138" + std::to_string(10000000 + user_id));
    user_info.set_avatar_url("https://example.com/avatars/user_" + std::to_string(user_id) + ".jpg");
    
    // 设置创建时间
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%SZ");
    user_info.set_created_at(ss.str());
    
    // 设置最后登录时间（模拟为当前时间减去一些随机时间）
    auto last_login = now - std::chrono::hours(rand() % 24) - std::chrono::minutes(rand() % 60);
    auto last_login_time_t = std::chrono::system_clock::to_time_t(last_login);
    std::stringstream ss2;
    ss2 << std::put_time(std::gmtime(&last_login_time_t), "%Y-%m-%dT%H:%M:%SZ");
    user_info.set_last_login(ss2.str());
    
    LOG_INFO("服务层：用户数据生成完成，用户ID: " + std::to_string(user_id));
    
    return user_info;
}

} // namespace user_service::service 