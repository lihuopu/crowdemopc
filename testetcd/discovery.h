#pragma once
#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <mutex>
#include <algorithm> // 新增：用于std::remove_if
#include <etcd/Client.hpp>
#include <etcd/Watcher.hpp>
#include <etcd/Response.hpp>

namespace chat_server {

class Discovery {
public:
    using ptr = std::shared_ptr<Discovery>;
    // 服务变化回调：key是服务路径，value是服务信息
    using OnServiceChanged = std::function<void(const std::string&, const std::string&)>;

    // 构造函数：初始化客户端并监听服务变化
    Discovery(const std::string& etcd_endpoint, 
             const std::string& service_name,
             OnServiceChanged on_add,    // 服务上线回调
             OnServiceChanged on_remove) // 服务下线回调
        : client_(std::make_shared<etcd::Client>(etcd_endpoint)),
          service_prefix_("/services/" + service_name + "/"),
          on_add_(on_add),
          on_remove_(on_remove) {
        // 1. 初始获取服务列表
        load_services();
        // 2. 启动监听（递归监听服务前缀下的所有变化）
        watcher_ = std::make_shared<etcd::Watcher>(*client_.get(), service_prefix_, 
            std::bind(&Discovery::on_watch_event, this, std::placeholders::_1), 
            true); // recursive=true：递归监听子节点
    }

    // 析构函数：停止监听
    ~Discovery() {
        if (watcher_) {
            watcher_->Cancel(); // 停止监听
        }
    }

    // 获取当前所有服务
    std::vector<std::pair<std::string, std::string>> get_services() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return services_;
    }

private:
    // 初始加载服务列表
    void load_services() {
        etcd::Response response = client_->ls(service_prefix_, true).get(); // 递归获取子节点
        if (!response.is_ok()) {
            std::cerr << "初始加载服务失败: " << response.error_message() << std::endl;
            return;
        }

        std::lock_guard<std::mutex> lock(mtx_);
        services_.clear();
        // 遍历所有服务节点
        for (size_t i = 0; i < response.keys().size(); ++i) {
            std::string key = response.key(i);
            std::string value = response.value(i).as_string();
            services_.emplace_back(key, value);
            if (on_add_) {
                on_add_(key, value); // 触发上线回调
            }
        }
    }

    // 监听事件回调（服务上线/下线）
    void on_watch_event(const etcd::Response& response) {
        if (!response.is_ok()) {
            std::cerr << "监听事件错误: " << response.error_message() << std::endl;
            return;
        }

        // 处理所有事件
        for (const auto& event : response.events()) {
            // 修正1：显式指定枚举类型（解决PUT/DELETE_未找到的问题）
            if (event.event_type() == etcd::Event::EventType::PUT) {
                // 服务上线/更新
                std::lock_guard<std::mutex> lock(mtx_);
                std::string key = event.kv().key();
                std::string value = event.kv().as_string();
                services_.emplace_back(key, value);
                if (on_add_) {
                    on_add_(key, value);
                }
                std::cout << "服务上线: " << key << " -> " << value << std::endl;
            } 
            else if (event.event_type() == etcd::Event::EventType::DELETE_) {
                // 服务下线
                std::lock_guard<std::mutex> lock(mtx_);
                std::string key = event.kv().key();
                // 修正2：lambda参数指定具体类型（兼容C++11，不使用auto）
                auto it = std::remove_if(services_.begin(), services_.end(),
                    [&](const std::pair<std::string, std::string>& pair) { 
                        return pair.first == key; 
                    });
                if (it != services_.end()) {
                    services_.erase(it, services_.end());
                    if (on_remove_) {
                        on_remove_(key, "");
                    }
                    std::cout << "服务下线: " << key << std::endl;
                }
            }
        }
    }

private:
    std::shared_ptr<etcd::Client> client_;         // etcd客户端
    std::shared_ptr<etcd::Watcher> watcher_;       // 监听器
    std::string service_prefix_;                   // 服务前缀（/services/服务名/）
    // 服务列表：key是路径，value是服务信息
    std::vector<std::pair<std::string, std::string>> services_; 
    mutable std::mutex mtx_;                       // 保护服务列表的锁
    OnServiceChanged on_add_;                      // 服务上线回调
    OnServiceChanged on_remove_;                   // 服务下线回调
};

} // namespace chat_server