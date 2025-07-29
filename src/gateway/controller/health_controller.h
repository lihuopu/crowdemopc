#pragma once

#include "crow/json.h"
#include <string>

namespace gateway::controller {

class HealthController {
public:
    static crow::json::wvalue health_check(const std::string& request_id);
};

} // namespace gateway::controller 