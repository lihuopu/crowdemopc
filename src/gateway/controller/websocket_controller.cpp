#include "websocket_controller.h"
#include "../util/logger.h"
#include "../util/response_util.h"
#include "crow/json.h"
#include <chrono>
#include <random>
#include <sstream>
#include <iomanip>

namespace gateway::controller {

// 静态成员变量定义
std::unordered_set<crow::websocket::connection*> WebSocketController::active_connections_;

void WebSocketController::handleWebSocketConnection(crow::websocket::connection& conn) {
    // 生成客户端ID
    std::string client_id = generateClientId();
    
    // 将连接添加到活跃连接集合
    active_connections_.insert(&conn);
    
    // 发送欢迎消息
    crow::json::wvalue welcome_msg;
    welcome_msg["type"] = "connection";
    welcome_msg["status"] = "connected";
    welcome_msg["client_id"] = client_id;
    welcome_msg["message"] = "欢迎连接到WebSocket服务！";
    welcome_msg["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    conn.send_text(welcome_msg.dump());
    
    // 广播新客户端连接消息
    crow::json::wvalue broadcast_msg;
    broadcast_msg["type"] = "system";
    broadcast_msg["message"] = "新客户端已连接，当前连接数: " + std::to_string(active_connections_.size());
    broadcast_msg["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    broadcastMessage(broadcast_msg.dump());
    
    LOG_INFO("WebSocket客户端连接成功，客户端ID: " + client_id + 
             "，当前连接数: " + std::to_string(active_connections_.size()));
}

void WebSocketController::handleWebSocketMessage(crow::websocket::connection& conn, const std::string& data, bool is_binary) {
    if (is_binary) {
        // 处理二进制消息
        conn.send_text("不支持二进制消息");
        return;
    }
    
    try {
        // 解析JSON消息
        auto json_data = crow::json::load(data);
        if (!json_data) {
            conn.send_text("无效的JSON格式");
            return;
        }
        
        // 获取消息类型
        std::string msg_type;
        if (json_data.has("type")) {
            msg_type = json_data["type"].s();
        } else {
            conn.send_text("消息缺少type字段");
            return;
        }
        
        // 根据消息类型处理
        if (msg_type == "chat") {
            handleChatMessage(conn, data);
        } else if (msg_type == "echo") {
            handleEchoMessage(conn, data);
        } else if (msg_type == "heartbeat") {
            handleHeartbeat(conn);
        } else {
            conn.send_text("未知的消息类型: " + msg_type);
        }
        
    } catch (const std::exception& e) {
        conn.send_text("处理消息时发生错误: " + std::string(e.what()));
        LOG_ERROR("WebSocket消息处理错误: " + std::string(e.what()));
    }
}

void WebSocketController::handleWebSocketClose(crow::websocket::connection& conn, const std::string& reason) {
    // 从活跃连接集合中移除
    active_connections_.erase(&conn);
    
    // 广播客户端断开连接消息
    crow::json::wvalue broadcast_msg;
    broadcast_msg["type"] = "system";
    broadcast_msg["message"] = "客户端已断开连接，当前连接数: " + std::to_string(active_connections_.size());
    broadcast_msg["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    broadcastMessage(broadcast_msg.dump());
    
    LOG_INFO("WebSocket客户端断开连接，原因: " + reason + 
             "，当前连接数: " + std::to_string(active_connections_.size()));
}

void WebSocketController::broadcastMessage(const std::string& message) {
    for (auto* conn : active_connections_) {
        if (conn) {
            conn->send_text(message);
        }
    }
}

void WebSocketController::sendMessageToClient(const std::string& client_id, const std::string& message) {
    // 这里可以实现向特定客户端发送消息的逻辑
    // 需要维护客户端ID到连接的映射
    LOG_INFO("向客户端 " + client_id + " 发送消息: " + message);
}

size_t WebSocketController::getConnectedClientsCount() {
    return active_connections_.size();
}

std::string WebSocketController::generateClientId() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(1000, 9999);
    
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << "client_" << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
    ss << "_" << std::setfill('0') << std::setw(3) << ms.count();
    ss << "_" << dis(gen);
    
    return ss.str();
}

void WebSocketController::handleChatMessage(crow::websocket::connection& conn, const std::string& data) {
    auto json_data = crow::json::load(data);
    
    std::string username = "匿名用户";
    std::string message = "";
    
    if (json_data.has("username")) {
        username = json_data["username"].s();
    }
    
    if (json_data.has("message")) {
        message = json_data["message"].s();
    }
    
    if (message.empty()) {
        conn.send_text("聊天消息不能为空");
        return;
    }
    
    // 构建聊天消息
    crow::json::wvalue chat_msg;
    chat_msg["type"] = "chat";
    chat_msg["username"] = username;
    chat_msg["message"] = message;
    chat_msg["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    // 广播聊天消息
    broadcastMessage(chat_msg.dump());
    
    LOG_INFO("聊天消息 - 用户: " + username + ", 消息: " + message);
}

void WebSocketController::handleEchoMessage(crow::websocket::connection& conn, const std::string& data) {
    auto json_data = crow::json::load(data);
    
    std::string message = "";
    if (json_data.has("message")) {
        message = json_data["message"].s();
    }
    
    // 构建回显消息
    crow::json::wvalue echo_msg;
    echo_msg["type"] = "echo";
    echo_msg["message"] = message;
    echo_msg["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    // 发送回显消息给发送者
    conn.send_text(echo_msg.dump());
    
    LOG_INFO("回显消息: " + message);
}

void WebSocketController::handleHeartbeat(crow::websocket::connection& conn) {
    // 构建心跳响应
    crow::json::wvalue heartbeat_msg;
    heartbeat_msg["type"] = "heartbeat";
    heartbeat_msg["status"] = "pong";
    heartbeat_msg["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    conn.send_text(heartbeat_msg.dump());
}

} // namespace gateway::controller 