#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include "service/grpc_service_impl.h"
#include "util/logger.h"
#include <iostream>
#include <signal.h>
#include <memory>

std::unique_ptr<grpc::Server> g_server;

void signalHandler(int signum) {
    LOG_INFO("收到信号 " + std::to_string(signum) + "，正在关闭服务器...");
    if (g_server) {
        g_server->Shutdown();
    }
}

int main() {
    // 初始化日志系统
    user_service::util::Logger::getInstance().init();
    
    // 注册信号处理器
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    std::string server_address("0.0.0.0:50051");
    
    // 创建服务实现
    user_service::service::GrpcServiceImpl service;
    
    // 启用 gRPC 反射服务（用于调试）
    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    
    // 创建服务器构建器
    grpc::ServerBuilder builder;
    
    // 添加监听端口
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    
    // 注册服务
    builder.RegisterService(&service);
    
    // 构建并启动服务器
    LOG_INFO("用户服务启动中，监听地址: " + server_address);
    
    g_server = builder.BuildAndStart();
    
    if (!g_server) {
        LOG_ERROR("服务器启动失败");
        return 1;
    }
    
    LOG_INFO("用户服务启动成功，等待请求...");
    
    // 等待服务器关闭
    g_server->Wait();
    
    LOG_INFO("用户服务已关闭");
    user_service::util::Logger::getInstance().shutdown();
    
    return 0;
} 