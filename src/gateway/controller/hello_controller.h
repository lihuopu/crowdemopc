#pragma once

#include "crow/json.h"
#include <string>

namespace gateway::controller {

class HelloController {
public:
    static crow::json::wvalue hello(const std::string& request_id);
    static crow::json::wvalue hello_with_name(const std::string& request_id, const std::string& name);
};

} // namespace gateway::controller 