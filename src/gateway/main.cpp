#include "crow.h"
#include "crow/middlewares/cors.h"
#include "middleware/route_middleware.h"
#include "controller/route_config.h"
#include "util/logger.h"
#include "util/config.h"
#include <signal.h>

int main() {
    // 加载配置文件
    auto& config_manager = gateway::util::ConfigManager::getInstance();
    if (!config_manager.loadConfig("../src/gateway/conf/server.yaml")) {
        std::cerr << "Failed to load configuration file" << std::endl;
        return 1;
    }
    
    // 从配置初始化日志系统
    const auto& log_config = config_manager.getLogConfig();
    gateway::util::Logger::getInstance().initFromConfig(log_config);
    
    LOG_INFO("Gateway service starting...");
    
    // 从配置获取服务器设置
    const auto& server_config = config_manager.getServerConfig();
    
    crow::App<RouteMiddleware, crow::CORSHandler> app;

    gateway::controller::RouteConfig::setup_routes(app);

    app.port(server_config.port)
       .multithreaded()
       .run();
    
    return 0;
}