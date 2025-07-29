#pragma once

#include "crow.h"
#include <string>
#include <memory>
#include <unordered_set>

namespace gateway::controller {

class WebSocketController {
public:
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
    
    // 生成唯一的客户端ID
    static std::string generateClientId();
    
    // 处理不同类型的消息
    static void handleChatMessage(crow::websocket::connection& conn, const std::string& data);
    static void handleEchoMessage(crow::websocket::connection& conn, const std::string& data);
    static void handleHeartbeat(crow::websocket::connection& conn);
};

} // namespace gateway::controller 