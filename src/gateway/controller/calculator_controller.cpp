#include "calculator_controller.h"
#include "../service/calculator_service.h"

namespace gateway::controller {

crow::json::wvalue CalculatorController::calculate(const std::string& request_id, const std::string& operation, int a, int b) {
    return gateway::service::CalculatorService::getInstance().performCalculation(request_id, operation, a, b);
}

} // namespace gateway::controller 