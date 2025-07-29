#include "api_controller.h"
#include "../service/api_service.h"

namespace gateway::controller {

crow::json::wvalue ApiController::post_data(const std::string& request_id, const crow::request& req) {
    return gateway::service::ApiService::getInstance().processPostData(request_id, req);
}

crow::json::wvalue ApiController::get_user_info(const std::string& request_id, int user_id) {
    return gateway::service::ApiService::getInstance().getUserInfo(request_id, user_id);
}

} // namespace gateway::controller 