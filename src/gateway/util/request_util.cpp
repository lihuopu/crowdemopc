#include "request_util.h"

// 线程局部存储的request_id
thread_local std::string current_request_id;

namespace gateway::util {

std::string generate_request_id() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static const char* hex_chars = "0123456789abcdef";
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    // 生成32位十六进制字符串
    for (int i = 0; i < 32; ++i) {
        ss << hex_chars[dis(gen)];
    }
    
    return ss.str();
}

std::string get_current_request_id() {
    return current_request_id;
}

void set_current_request_id(const std::string& request_id) {
    current_request_id = request_id;
}

} // namespace gateway::util 