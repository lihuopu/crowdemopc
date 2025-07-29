#include "health_controller.h"
#include "../service/health_service.h"

namespace gateway::controller {

crow::json::wvalue HealthController::health_check(const std::string& request_id) {
    return gateway::service::HealthService::getInstance().performHealthCheck(request_id);
}

} // namespace gateway::controller 