#pragma once

#include "crow/json.h"
#include <string>

namespace gateway::util {

// 错误响应工具函数
crow::json::wvalue error_response(const std::string& error_msg, int status_code = 400);

// 成功响应工具函数
crow::json::wvalue success_response(const std::string& message);

} // namespace gateway::util 