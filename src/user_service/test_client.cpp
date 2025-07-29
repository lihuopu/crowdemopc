#include "service/user_client.h"
#include "util/logger.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    // 初始化日志系统
    user_service::util::Logger::getInstance().init();
    
    LOG_INFO("用户服务测试客户端启动...");
    
    // 创建客户端
    user_service::service::UserClient client("localhost:50051");
    
    if (!client.IsConnected()) {
        LOG_ERROR("无法连接到用户服务，请确保服务已启动");
        return 1;
    }
    
    LOG_INFO("成功连接到用户服务");
    
    // 测试不同的用户ID
    std::vector<int32_t> test_user_ids = {1, 100, 999, 1000, 100001};
    
    for (int32_t user_id : test_user_ids) {
        LOG_INFO("测试用户ID: " + std::to_string(user_id));
        
        auto [success, response] = client.GetUserInfo(user_id);
        
        if (success) {
            if (response.success()) {
                LOG_INFO("✓ 成功获取用户信息");
                std::cout << "  用户名: " << response.user_info().username() << std::endl;
                std::cout << "  邮箱: " << response.user_info().email() << std::endl;
                std::cout << "  状态: " << response.user_info().status() << std::endl;
                std::cout << "  全名: " << response.user_info().full_name() << std::endl;
                std::cout << "  电话: " << response.user_info().phone() << std::endl;
            } else {
                LOG_WARN("✗ 获取用户信息失败: " + response.message());
            }
        } else {
            LOG_ERROR("✗ 客户端调用失败");
        }
        
        // 等待一下再测试下一个
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    LOG_INFO("测试完成");
    user_service::util::Logger::getInstance().shutdown();
    return 0;
} 