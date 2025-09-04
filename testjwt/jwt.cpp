#include <jwt-cpp/jwt.h>       // JWT库头文件，提供生成和解析JWT的功能
#include <chrono>              // 时间库，用于处理时间戳和时间间隔
#include <ctime>               // 时间相关函数
#include <string>              // 字符串处理
#include <iostream>            // 输入输出流

// 使用标准命名空间，简化代码书写
using namespace std;

/**
 * 生成JWT令牌
 * @param device_id 设备ID，作为自定义声明放入JWT载荷
 * @param serial_number 序列号，作为自定义声明放入JWT载荷
 * @return 生成的JWT令牌字符串
 */
std::string generate_jwt(const string& device_id, const string& serial_number) {
    // 获取当前系统时间（UTC时间）
    auto now = chrono::system_clock::now();
    
    // 计算当前时间戳（秒级）：从纪元时间(1970-01-01 00:00:00)到现在的秒数
    int64_t iat_ts = chrono::duration_cast<chrono::seconds>(
        now.time_since_epoch()
    ).count();
    
    // 计算过期时间戳：当前时间+3600秒（1小时后过期）
    int64_t exp_ts = iat_ts + 3600;

    // 将时间戳转换为jwt库要求的date类型（系统时间点）
    // 注意：from_time_t参数是time_t类型（秒级时间戳），直接兼容int64_t
    jwt::date iat = chrono::system_clock::from_time_t(iat_ts);  // 签发时间
    jwt::date exp = chrono::system_clock::from_time_t(exp_ts);  // 过期时间

    // 创建并配置JWT令牌
    auto token = jwt::create()
        .set_issuer("issuer")                     // 设置签发者(iss声明)
        .set_type("JWT")                          // 设置令牌类型(typ声明)
        .set_payload_claim("device_id", jwt::claim(device_id))      // 添加自定义声明：设备ID
        .set_payload_claim("serial_number", jwt::claim(serial_number))  // 添加自定义声明：序列号
        .set_payload_claim("iat", jwt::claim(iat))  // 添加签发时间(iat声明)
        .set_payload_claim("exp", jwt::claim(exp))  // 添加过期时间(exp声明)
        .sign(jwt::algorithm::hs256{"secret_key"});  // 使用HS256算法和密钥签名

    return token;  // 返回生成的JWT字符串
}

/**
 * 验证JWT令牌的有效性
 * @param token 待验证的JWT令牌字符串
 * @return 验证成功返回true，否则返回false
 */
bool verify_jwt(const string& token) {
    try {
        // 创建JWT验证器并配置验证规则
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{"secret_key"})  // 允许的签名算法和密钥
            .with_issuer("issuer");  // 验证签发者(iss声明)必须为"issuer"

        // 解析JWT令牌（仅解析不验证，验证由verifier完成）
        auto decoded = jwt::decode(token);
        
        // 执行验证：包括签名验证、声明验证（如issuer）等
        verifier.verify(decoded);

        // 手动验证过期时间（虽然verifier可能已验证，但显式验证更清晰）
        auto exp_claim = decoded.get_payload_claim("exp");  // 获取exp声明
        auto exp_time = exp_claim.as_date();  // 将exp声明转换为时间点类型
        auto now = chrono::system_clock::now();  // 获取当前时间
        if (exp_time < now) {  // 若过期时间早于当前时间，说明令牌已过期
            cerr << "Token expired" << endl;
            return false;
        }

        // 提取并打印JWT中的自定义声明
        std::string device_id = decoded.get_payload_claim("device_id").as_string();  // 获取设备ID
        std::string serial_number = decoded.get_payload_claim("serial_number").as_string();  // 获取序列号
        
        // 提取签发时间并转换为秒级时间戳
        auto iat_time = decoded.get_payload_claim("iat").as_date();  // 获取iat声明（时间点类型）
        int64_t iat_ts = chrono::duration_cast<chrono::seconds>(
            iat_time.time_since_epoch()  // 转换为从纪元时间开始的秒数
        ).count();

        // 打印解析结果
        cout << "device_id: " << device_id << endl;
        cout << "serial_number: " << serial_number << endl;
        cout << "iat: " << iat_ts << endl;

        return true;  // 所有验证通过，返回true
    } catch (const exception& e) {
        // 捕获验证过程中的异常（如签名错误、声明不匹配、格式错误等）
        cerr << "Verification failed: " << e.what() << endl;
        return false;
    }
}

/**
 * 主函数：演示JWT的生成和验证过程
 */
int main() {
    // 示例参数：设备ID和序列号
    string device_id = "device_123";
    string serial = "serial_456";
    
    // 生成JWT令牌
    string token = generate_jwt(device_id, serial);
    cout << "Generated token: " << token << endl;

    // 验证JWT令牌并输出结果
    bool valid = verify_jwt(token);
    cout << "Token is " << (valid ? "valid" : "invalid") << endl;
    
    return 0;
}
