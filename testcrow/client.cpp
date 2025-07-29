#include <iostream>
#include <string>
#include <curl/curl.h>

// 回调函数：用于处理libcurl接收到的响应数据
// 参数说明：
// - contents：指向接收到的数据的指针（类型为void*，需转换为char*）
// - size：每个数据块的大小（字节）
// - nmemb：数据块的数量
// - s：指向存储响应内容的std::string对象的指针
// 返回值：实际处理的字节数（必须等于size*nmemb，否则libcurl会认为传输错误）
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    // 计算接收到的数据的总字节数
    size_t newLength = size * nmemb;
    try {
        // 将接收到的数据（char*类型）追加到std::string对象中
        // 使用append方法可以高效地扩展字符串，避免频繁内存重新分配
        s->append((char*)contents, newLength);
    } catch (std::bad_alloc& e) {
        // 处理内存分配失败的异常
        // 返回0告知libcurl终止传输，避免程序崩溃
        return 0;
    }
    // 返回实际处理的字节数，必须等于newLength
    // 这是libcurl回调函数的约定，用于确认数据完整接收
    return newLength;
}

// 发送HTTP GET请求
std::string httpGet(const std::string& url) {
    //初始化libcurl会话
    CURL* curl = curl_easy_init();
    //存储响应内容的字符串
    std::string response_string;
    
    if(curl) {
        //设置请求URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        //设置响应数据的回调函数
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
         // 将回调函数的输出写入response_string
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        
        // 执行HTTP请求
        CURLcode res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            // 请求失败时输出错误信息
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        
        curl_easy_cleanup(curl);
    }
    return response_string;
}

// 发送HTTP POST请求
std::string httpPost(const std::string& url, const std::string& postData) {
    CURL* curl = curl_easy_init();
    std::string response_string;
    
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        // 设置POST请求的请求体数据
        // CURLOPT_POSTFIELDS会自动设置POST方法，并指定请求体内容
        // libcurl会自动计算Content-Length头
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
        // 设置响应数据的回调函数
        // 当libcurl接收到响应数据时，会调用WriteCallback函数处理
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        
        CURLcode res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        
        curl_easy_cleanup(curl);
    }
    
    return response_string;
}

int main() {
    // 初始化libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    
    // 1. 测试GET请求 - 获取所有用户
    std::cout << "GET /api/users" << std::endl;
    std::string response = httpGet("http://localhost:8080/api/users");
    std::cout << "Response: " << response << std::endl << std::endl;
    
    // 2. 测试GET请求 - 获取单个用户
    std::cout << "GET /api/users/1" << std::endl;
    response = httpGet("http://localhost:8080/api/users/1");
    std::cout << "Response: " << response << std::endl << std::endl;
    
    // 3. 测试POST请求 - 创建新用户
    std::cout << "POST /api/users" << std::endl;
    //R 是关键字，标记这是一个原始字符串。
    //"(...) 是定界符：( 和 ) 之间的内容会被原样解析，不需要对特殊字符进行转义。
    std::string postData = R"({"name": "Charlie", "email": "charlie@example.com"})";
    response = httpPost("http://localhost:8080/api/users", postData);
    std::cout << "Response: " << response << std::endl << std::endl;
    
    // 4. 再次获取所有用户，验证创建操作
    std::cout << "GET /api/users (after POST)" << std::endl;
    response = httpGet("http://localhost:8080/api/users");
    std::cout << "Response: " << response << std::endl;
    
    // 清理libcurl资源
    curl_global_cleanup();
    
    return 0;
}