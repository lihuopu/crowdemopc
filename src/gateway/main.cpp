#include "crow.h"
#include "crow/middlewares/cors.h"
#include "middleware/route_middleware.h"
#include "controller/route_config.h"
#include "service/api_service.h"
#include "util/logger.h"
#include "util/config.h"
#include <signal.h>

int main() {
    // 加载配置文件
    auto& config_manager = gateway::util::ConfigManager::getInstance();
    if (!config_manager.loadConfig("../conf/server.yaml")) {
        std::cerr << "Failed to load configuration file" << std::endl;
        return 1;
    }
    
    // 从配置初始化日志系统
    const auto& log_config = config_manager.getLogConfig();
    gateway::util::Logger::getInstance().initFromConfig(log_config);
    
    LOG_INFO("Gateway service starting...");

    // // 【关键步骤1：提前初始化ApiService，触发服务发现】
    // LOG_INFO("手动触发ApiService初始化（开始服务发现）...");
    // auto& api_service = gateway::service::ApiService::getInstance();  // 显式构造单例

    // // 【关键步骤2：强制延迟10秒，给服务发现足够时间完成（无需新增方法）】
    // LOG_INFO("等待10秒，让服务发现完成...");
    // std::this_thread::sleep_for(std::chrono::seconds(10));  //
    
    // 从配置获取服务器设置
    const auto& server_config = config_manager.getServerConfig();
    
    crow::App<RouteMiddleware, crow::CORSHandler> app;

    gateway::controller::RouteConfig::setup_routes(app);

    app.port(server_config.port)
       .multithreaded()
       .run();
    
    return 0;
}