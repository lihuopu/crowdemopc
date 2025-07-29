#include "calculator_service.h"
#include "../util/logger.h"
#include <chrono>
#include <stdexcept>

namespace gateway::service {

CalculatorService& CalculatorService::getInstance() {
    static CalculatorService instance;
    return instance;
}

crow::json::wvalue CalculatorService::performCalculation(const std::string& request_id, const std::string& operation, int a, int b) {
    LOG_INFO("RequestID: " + request_id + 
             " | Processing calculation: " + operation + "(" + std::to_string(a) + ", " + std::to_string(b) + ")");
    
    crow::json::wvalue response;
    response["request_id"] = request_id;
    
    // 验证操作
    if (!validateOperation(operation)) {
        response["success"] = false;
        response["error"] = "不支持的操作: " + operation;
        response["status_code"] = 400;
        response["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        return response;
    }
    
    // 验证操作数
    if (!validateOperands(operation, a, b)) {
        response["success"] = false;
        response["error"] = "无效的操作数";
        response["status_code"] = 400;
        response["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        return response;
    }
    
    // 检查除零错误
    if (checkDivisionByZero(operation, b)) {
        response["success"] = false;
        response["error"] = "除数不能为零";
        response["status_code"] = 400;
        response["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        return response;
    }
    
    try {
        double result = executeCalculation(operation, a, b);
        
        response["success"] = true;
        response["message"] = "计算完成";
        response["data"]["operation"] = operation;
        response["data"]["a"] = a;
        response["data"]["b"] = b;
        response["data"]["result"] = result;
        
        LOG_INFO("RequestID: " + request_id + " | Calculation completed successfully: " + std::to_string(result));
    } catch (const std::exception& e) {
        response["success"] = false;
        response["error"] = "计算错误: " + std::string(e.what());
        response["status_code"] = 500;
        LOG_ERROR("RequestID: " + request_id + " | Calculation error: " + e.what());
    }
    
    response["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    return response;
}

bool CalculatorService::validateOperation(const std::string& operation) {
    return operation == "add" || operation == "subtract" || 
           operation == "multiply" || operation == "divide";
}

bool CalculatorService::validateOperands(const std::string& operation, int a, int b) {
    // 基本验证：检查是否为有效数字
    // 这里可以添加更复杂的验证逻辑
    return true;
}

double CalculatorService::executeCalculation(const std::string& operation, int a, int b) {
    if (operation == "add") {
        return a + b;
    } else if (operation == "subtract") {
        return a - b;
    } else if (operation == "multiply") {
        return a * b;
    } else if (operation == "divide") {
        if (b == 0) {
            throw std::invalid_argument("Division by zero");
        }
        return static_cast<double>(a) / b;
    } else {
        throw std::invalid_argument("Unsupported operation: " + operation);
    }
}

bool CalculatorService::checkDivisionByZero(const std::string& operation, int b) {
    return operation == "divide" && b == 0;
}

} // namespace gateway::service 