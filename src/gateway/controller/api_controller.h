#pragma once

#include "crow/json.h"
#include "crow/http_request.h"
#include <string>

namespace gateway::controller {

class ApiController {
public:
    static crow::json::wvalue post_data(const std::string& request_id, const crow::request& req);
    static crow::json::wvalue get_user_info(const std::string& request_id, int user_id);
};

} // namespace gateway::controller 
