#pragma once

#include "crow/json.h"
#include <string>

namespace gateway::service {

class CalculatorService {
public:
    static CalculatorService& getInstance();
    
    // 执行计算
    crow::json::wvalue performCalculation(const std::string& request_id, const std::string& operation, int a, int b);
    
    // 验证操作
    bool validateOperation(const std::string& operation);
    
    // 验证操作数
    bool validateOperands(const std::string& operation, int a, int b);

private:
    CalculatorService() = default;
    ~CalculatorService() = default;
    CalculatorService(const CalculatorService&) = delete;
    CalculatorService& operator=(const CalculatorService&) = delete;
    
    // 执行具体的计算
    double executeCalculation(const std::string& operation, int a, int b);
    
    // 检查除零错误
    bool checkDivisionByZero(const std::string& operation, int b);
};

} // namespace gateway::service 