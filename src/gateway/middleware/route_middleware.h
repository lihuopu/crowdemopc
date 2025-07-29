#pragma once

#include "crow/http_request.h"
#include "crow/http_response.h"
#include "../util/request_util.h"
#include "../util/logger.h"
#include <chrono>
#include <string>

// filter
// middlerware  pre --> api --> after

// 路由通用中间件
struct RouteMiddleware {
    struct context {
        std::string request_id;
        std::chrono::high_resolution_clock::time_point start_time;
    };

    void before_handle(crow::request& req, crow::response& res, context& ctx) { 
        // 生成request_id
        ctx.request_id = gateway::util::generate_request_id();
        ctx.start_time = std::chrono::high_resolution_clock::now();
        
        // 设置当前请求的request_id
        gateway::util::set_current_request_id(ctx.request_id);
        
        // 记录请求信息到日志文件
        LOG_INFO("RequestID: " + ctx.request_id + 
                 " | Method: " + std::to_string(static_cast<int>(req.method)) + 
                 " | URL: " + req.url);
        
        // 记录请求头
        LOG_INFO("RequestID: " + ctx.request_id + " | Headers:");
        for (const auto& header : req.headers) {
            LOG_INFO("  " + header.first + ": " + header.second);
        }
        
        // 记录请求体（如果是JSON）
        if (!req.body.empty()) {
            LOG_INFO("RequestID: " + ctx.request_id + " | Body:");
            LOG_INFO("  " + req.body);
            
            // 尝试解析并格式化JSON
            try {
                auto json_body = crow::json::load(req.body);
                if (json_body) {
                    LOG_INFO("RequestID: " + ctx.request_id + " | Parsed JSON:");
                    LOG_INFO("  " + std::string(json_body));
                }
            } catch (const std::exception& e) {
                LOG_WARN("RequestID: " + ctx.request_id + 
                         " | Failed to parse JSON: " + e.what());
            }
        }
        
        LOG_INFO("RequestID: " + ctx.request_id + " | 开始处理请求");
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx) {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - ctx.start_time);
        
        LOG_INFO("RequestID: " + ctx.request_id + 
                 " | Method: " + std::to_string(static_cast<int>(req.method)) + 
                 " | URL: " + req.url + 
                 " | 处理完成，耗时: " + std::to_string(duration.count()) + "ms");
        
        // 记录响应状态
        LOG_INFO("RequestID: " + ctx.request_id + 
                 " | Response Status: " + std::to_string(res.code));
    }
}; 