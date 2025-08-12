#pragma once

#include "crow.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "../../../testredis/redis.hpp"  // 引入Redis客户端

namespace gateway::controller {

class WebSocketController {
public:
    // 初始化Redis客户端（在服务启动时调用）
    static void initRedisClient();

    // WebSocket连接处理
    static void handleWebSocketConnection(crow::websocket::connection& conn);
    
    // 处理WebSocket消息
    static void handleWebSocketMessage(crow::websocket::connection& conn, const std::string& data, bool is_binary);
    
    // 处理WebSocket关闭
    static void handleWebSocketClose(crow::websocket::connection& conn, const std::string& reason);

    // 广播消息给所有连接的客户端
    static void broadcastMessage(const std::string& message);
    
    // 发送消息给特定客户端
    static void sendMessageToClient(const std::string& client_id, const std::string& message);
    
    // 获取当前连接的客户端数量
    static size_t getConnectedClientsCount();

private:
    // 存储所有活跃的WebSocket连接
    static std::unordered_set<crow::websocket::connection*> active_connections_;
    // 线程安全锁
    static std::mutex mutex_;

    // Redis客户端实例
    static std::unique_ptr<Redis> redis_client_;
    // 生成唯一的客户端ID
    static std::string generateClientId();

    // Redis消息回调：收到消息后转发到对应通道的客户端
    static void onRedisMessage(int channel, const std::string& message);
    
    // 处理不同类型的消息
    static void handleChatMessage(crow::websocket::connection& conn, const std::string& data);
    static void handleEchoMessage(crow::websocket::connection& conn, const std::string& data);
    static void handleHeartbeat(crow::websocket::connection& conn);
};

} // namespace gateway::controller 