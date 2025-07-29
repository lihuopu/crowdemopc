#include <crow.h>
#include <string>
#include <vector>
#include <map>

// 模拟数据库：存储用户数据的向量，每个用户用键值对映射表示
std::vector<std::map<std::string, std::string>> users = {
    {{"id", "1"}, {"name", "Alice"}, {"email", "alice@example.com"}},
    {{"id", "2"}, {"name", "Bob"}, {"email", "bob@example.com"}}
};

int main() {
    crow::SimpleApp app; // 创建Crow框架的应用实例

    // 根路由：返回欢迎信息
    CROW_ROUTE(app, "/")
    ([]() {
        return "Welcome to Crow Server!";
    });

    // 获取所有用户的API接口
    // 使用流式API构建JSON数组响应
    CROW_ROUTE(app, "/api/users")
    ([]() {
        crow::json::wvalue result = crow::json::wvalue::list();  // 创建JSON数组
        
        // 遍历所有用户，将每个用户转换为JSON对象
        for (const auto& user : users) {
            crow::json::wvalue userJson;
            for (const auto& pair : user) {
                userJson[pair.first] = pair.second;  // 填充用户属性
            }
            
            // 使用移动语义将用户对象添加到结果数组，避免拷贝开销
            result[result.size()] = std::move(userJson);
        }
        
        // 包装用户数组到顶层JSON对象
        crow::json::wvalue response;
        response["users"] = std::move(result);
        return crow::response(200, response);  // 返回HTTP 200 OK和JSON数据
    });

    // 根据ID获取单个用户的API接口
    CROW_ROUTE(app, "/api/users/<int>")
    ([](int id) {
        // 查找匹配ID的用户
        for (const auto& user : users) {
            auto it = user.find("id");
            if (it != user.end() && std::stoi(it->second) == id) {
                // 找到用户后构建JSON响应
                crow::json::wvalue userJson;
                for (const auto& pair : user) {
                    userJson[pair.first] = pair.second;
                }
                return crow::response(200, userJson);  // 返回200和用户数据
            }
        }
        
        // 未找到用户时返回404错误
        crow::json::wvalue error;
        error["error"] = "User not found";
        return crow::response(404, error);
    });

    // 创建新用户的API接口（处理POST请求）
    CROW_ROUTE(app, "/api/users").methods("POST"_method)
    ([](const crow::request& req) {
        // 解析请求中的JSON数据
        auto json_data = crow::json::load(req.body);
        if (!json_data) {
            crow::json::wvalue error;
            error["error"] = "Invalid JSON";
            return crow::response(400, error);  // 无效JSON格式返回400
        }

        // 验证必要字段
        if (!json_data.has("name") || !json_data.has("email")) {
            crow::json::wvalue error;
            error["error"] = "Missing 'name' or 'email'";
            return crow::response(400, error);  // 缺少必要字段返回400
        }

        // 创建新用户对象
        std::map<std::string, std::string> new_user;
        new_user["id"] = std::to_string(users.size() + 1);  // 自动生成ID
        new_user["name"] = json_data["name"].s();  // 提取名称字段
        new_user["email"] = json_data["email"].s();  // 提取邮箱字段
        users.push_back(new_user);  // 添加到用户列表

        // 返回创建成功响应
        crow::json::wvalue success;
        success["message"] = "User created";
        return crow::response(201, success);  // 返回201创建成功
    });

    // 启动服务器：监听8080端口，启用多线程模式
    app.port(8080).multithreaded().run();
    return 0;
}