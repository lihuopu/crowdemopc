#include "response_util.h"
#include <chrono>

namespace gateway::util {

crow::json::wvalue error_response(const std::string& error_msg, int status_code) {
    crow::json::wvalue response;
    response["error"] = error_msg;
    response["status_code"] = status_code;
    response["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    return response;
}

crow::json::wvalue success_response(const std::string& message) {
    crow::json::wvalue response;
    response["success"] = true;
    response["message"] = message;
    response["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    return response;
}

} // namespace gateway::util 