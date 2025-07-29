#pragma once

#include "crow/json.h"
#include <string>

namespace gateway::service {

class HealthService {
public:
    static HealthService& getInstance();
    
    // 健康检查
    crow::json::wvalue performHealthCheck(const std::string& request_id);
    
    // 获取服务信息
    crow::json::wvalue getServiceInfo(const std::string& request_id);

private:
    HealthService() = default;
    ~HealthService() = default;
    HealthService(const HealthService&) = delete;
    HealthService& operator=(const HealthService&) = delete;
};

} // namespace gateway::service 