#include "health_service.h"
#include "../util/logger.h"
#include <chrono>

namespace gateway::service {

HealthService& HealthService::getInstance() {
    static HealthService instance;
    return instance;
}

crow::json::wvalue HealthService::performHealthCheck(const std::string& request_id) {
    LOG_INFO("RequestID: " + request_id + " | Performing health check");
    
    crow::json::wvalue response;
    response["status"] = "healthy";
    response["service"] = "gateway";
    response["version"] = "1.0.0";
    response["request_id"] = request_id;
    response["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    LOG_INFO("RequestID: " + request_id + " | Health check completed successfully");
    return response;
}

crow::json::wvalue HealthService::getServiceInfo(const std::string& request_id) {
    LOG_INFO("RequestID: " + request_id + " | Getting service information");
    
    crow::json::wvalue response;
    response["service_name"] = "Gateway Service";
    response["version"] = "1.0.0";
    response["description"] = "API Gateway for AI Model Services";
    response["status"] = "running";
    response["request_id"] = request_id;
    response["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    return response;
}

} // namespace gateway::service 