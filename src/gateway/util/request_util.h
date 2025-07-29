#pragma once

#include <string>
#include <random>
#include <sstream>
#include <iomanip>

namespace gateway::util {

// 生成唯一的request_id
std::string generate_request_id();

// 获取当前请求的request_id（从线程局部存储）
std::string get_current_request_id();

// 设置当前请求的request_id
void set_current_request_id(const std::string& request_id);

} // namespace gateway::util 