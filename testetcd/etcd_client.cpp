#include "discovery.h"
#include <iostream>
#include <thread>

// 服务上线回调
void on_service_added(const std::string& key, const std::string& value) {
    std::cout << "检测到服务上线: " << key << " -> " << value << std::endl;
}

// 服务下线回调
void on_service_removed(const std::string& key, const std::string& value) {
    std::cout << "检测到服务下线: " << key << std::endl;
}

int main() {
    try {
        // 初始化服务发现器（连接etcd，监听"my_service"服务）
        chat_server::Discovery::ptr discovery = 
            std::make_shared<chat_server::Discovery>(
                "http://localhost:2379", 
                "my_service", 
                on_service_added, 
                on_service_removed
            );

        // 定期打印当前服务列表（每5秒）
        std::cout << "客户端运行中，按Enter退出..." << std::endl;
        while (true) {
            auto services = discovery->get_services();
            std::cout << "\n当前可用服务（" << services.size() << "个）:" << std::endl;
            // 修正3：不使用结构化绑定（兼容C++11）
            for (const auto& service : services) { 
                std::cout << service.first << " -> " << service.second << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    } catch (const std::exception& e) {
        std::cerr << "客户端启动失败: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}