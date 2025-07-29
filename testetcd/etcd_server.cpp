// server.cpp
#include "registry.h"
#include <iostream>
#include <thread>

int main() {
    try {
        // 初始化服务注册器（连接etcd，创建30秒租约）
        chat_server::Registry::ptr registry = 
            std::make_shared<chat_server::Registry>("http://localhost:2379", 30);

        // 注册服务（服务名、主机IP、端口）
        // 注意：主机IP需替换为实际可访问的IP（如ifconfig中的IP，不要用localhost）
        bool ok = registry->register_service("my_service", "192.168.58.130", 8080);
        if (!ok) {
            return 1;
        }

        // 保持服务运行（模拟业务逻辑）
        std::cout << "服务运行中，按Enter退出..." << std::endl;
        std::cin.get();
    } catch (const std::exception& e) {
        std::cerr << "服务启动失败: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}