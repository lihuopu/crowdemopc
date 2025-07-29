#pragma once

#include "crow/json.h"
#include <string>

namespace gateway::controller {

class CalculatorController {
public:
    static crow::json::wvalue calculate(const std::string& request_id, const std::string& operation, int a, int b);
};

} // namespace gateway::controller 