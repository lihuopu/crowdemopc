#pragma once

#include "crow.h"
#include "crow/middlewares/cors.h"
#include "../middleware/route_middleware.h"
#include "health_controller.h"
#include "hello_controller.h"
#include "calculator_controller.h"
#include "api_controller.h"
#include "websocket_controller.h"
#include "../util/response_util.h"
#include "../util/request_util.h"
#include <string>

namespace gateway::controller {

class RouteConfig {
public:
    static void setup_routes(crow::App<RouteMiddleware, crow::CORSHandler>& app) {
        // 配置CORS
        auto& cors = app.get_middleware<crow::CORSHandler>();
        cors
            .global()
            .headers("Content-Type")
            .methods("GET"_method, "POST"_method, "PUT"_method, "DELETE"_method);

        // 健康检查接口
        CROW_ROUTE(app, "/health").methods(crow::HTTPMethod::GET)
        ([]() {
            return HealthController::health_check(gateway::util::get_current_request_id());
        });

        // 基础问候接口
        CROW_ROUTE(app, "/hello").methods(crow::HTTPMethod::GET)
        ([]() {
            return HelloController::hello(gateway::util::get_current_request_id());
        });

        // 带参数的问候接口
        CROW_ROUTE(app, "/hello/<string>").methods(crow::HTTPMethod::GET)
        ([](const std::string& name) {
            return HelloController::hello_with_name(gateway::util::get_current_request_id(), name);
        });

        // 数学计算接口
        CROW_ROUTE(app, "/calculate/<string>/<int>/<int>").methods(crow::HTTPMethod::GET)
        ([](const std::string& operation, int a, int b) {
            return CalculatorController::calculate(gateway::util::get_current_request_id(), operation, a, b);
        });

        // POST接口示例
        CROW_ROUTE(app, "/api/data").methods(crow::HTTPMethod::POST)
        ([](const crow::request& req) {
            return ApiController::post_data(gateway::util::get_current_request_id(), req);
        });

        // 用户信息接口
        CROW_ROUTE(app, "/api/user/<int>").methods(crow::HTTPMethod::GET)
        ([](int user_id) {
            return ApiController::get_user_info(gateway::util::get_current_request_id(), user_id);
        });

        // WebSocket接口
        CROW_WEBSOCKET_ROUTE(app, "/ws")
        .onopen([](crow::websocket::connection& conn) {
            WebSocketController::handleWebSocketConnection(conn);
        })
        .onmessage([](crow::websocket::connection& conn, const std::string& data, bool is_binary) {
            WebSocketController::handleWebSocketMessage(conn, data, is_binary);
        })
        .onclose([](crow::websocket::connection& conn, const std::string& reason, uint16_t code) {
            WebSocketController::handleWebSocketClose(conn, reason);
        });

        // WebSocket状态接口
        CROW_ROUTE(app, "/ws/status").methods(crow::HTTPMethod::GET)
        ([]() {
            crow::json::wvalue response;
            response["success"] = true;
            response["connected_clients"] = static_cast<int>(WebSocketController::getConnectedClientsCount());
            response["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
            return response;
        });

        // 404处理
        CROW_CATCHALL_ROUTE(app)
        ([](crow::response& res) {
            crow::json::wvalue error_resp;
            error_resp["success"] = false;
            error_resp["error"] = "接口不存在";
            error_resp["status_code"] = 404;
            error_resp["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
            res = crow::response(404, error_resp);
            res.end();
        });
    }
};

} // namespace gateway::controller 