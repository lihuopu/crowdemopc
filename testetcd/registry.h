// registry.h
#pragma once
#include <string>
#include <memory>
#include <etcd/Client.hpp>
#include <etcd/KeepAlive.hpp>
#include <etcd/Response.hpp>

namespace chat_server {

class Registry {
public:
    using ptr = std::shared_ptr<Registry>;

    // 构造函数：初始化etcd客户端、创建租约并启动保活
    Registry(const std::string& etcd_endpoint, int ttl = 30) 
        : client_(std::make_shared<etcd::Client>(etcd_endpoint)),
          ttl_(ttl) {
        // 关键：通过leasekeepalive获取KeepAlive对象，再通过其Lease()方法获取租约ID
        keep_alive_ = client_->leasekeepalive(ttl_).get();
        lease_id_ = keep_alive_->Lease();  // 从KeepAlive中获取租约ID（核心修正）
        if (lease_id_ == 0) {
            throw std::runtime_error("获取租约ID失败");
        }
    }

    // 析构函数：停止保活
    ~Registry() {
        if (keep_alive_) {
            keep_alive_->Cancel();  // 停止保活，租约会自动过期
        }
    }

    // 注册服务：将服务信息写入etcd（绑定租约）
    bool register_service(const std::string& service_name, 
                         const std::string& host, 
                         int port) {
        // 服务存储路径：/services/服务名/主机:端口
        std::string key = "/services/" + service_name + "/" + host + ":" + std::to_string(port);
        // 服务信息（JSON格式）
        std::string value = "{\"host\":\"" + host + "\",\"port\":" + std::to_string(port) + "}";
        
        // 写入etcd，绑定租约ID
        etcd::Response response = client_->put(key, value, lease_id_).get();
        if (!response.is_ok()) {
            std::cerr << "服务注册失败: " << response.error_message() << std::endl;
            return false;
        }
        std::cout << "服务注册成功: " << key << " -> " << value << std::endl;
        return true;
    }

private:
    std::shared_ptr<etcd::Client> client_;       // etcd客户端
    std::shared_ptr<etcd::KeepAlive> keep_alive_; // 租约保活对象
    int64_t lease_id_;                           // 租约ID（通过KeepAlive获取）
    int ttl_;                                     // 租约有效期（秒）
};

} // namespace chat_server