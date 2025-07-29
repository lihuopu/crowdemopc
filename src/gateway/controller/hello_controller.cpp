#include "hello_controller.h"
#include "../service/hello_service.h"

namespace gateway::controller {

crow::json::wvalue HelloController::hello(const std::string& request_id) {
    return gateway::service::HelloService::getInstance().generateGreeting(request_id);
}

crow::json::wvalue HelloController::hello_with_name(const std::string& request_id, const std::string& name) {
    return gateway::service::HelloService::getInstance().generatePersonalizedGreeting(request_id, name);
}

} // namespace gateway::controller 