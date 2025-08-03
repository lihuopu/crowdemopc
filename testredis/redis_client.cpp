#include "redis.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

using namespace std;

//g++ redis_client.cpp redis.cpp -o redis_client -lhiredis -lpthread
// 消息处理回调函数
void handleMessage(int channel, string message) {
    cout << "\n收到来自通道 " << channel << " 的消息: " << message << endl;
    cout << "请输入命令(输入quit退出): ";
    cout.flush(); // 刷新输出缓冲区
}

int main() {
    // 创建Redis对象并连接服务器
    Redis redis;
    if (!redis.connect()) {
        cerr << "连接Redis服务器失败!" << endl;
        return 1;
    }
    
    // 设置消息处理回调函数
    redis.init_notify_handler(handleMessage);
    
    int channel = 100; // 要订阅的通道
    if (!redis.subscribe(channel)) {
        cerr << "订阅通道 " << channel << " 失败!" << endl;
        return 1;
    }
    
    cout << "Redis订阅者已启动，已订阅通道 " << channel << endl;
    cout << "请输入命令(输入quit退出): ";
    
    string command;
    while (true) {
        getline(cin, command);
        
        if (command == "quit") {
            // 取消订阅
            redis.unsubscribe(channel);
            break;
        } else {
            cout << "未知命令，请输入quit退出" << endl;
            cout << "请输入命令: ";
        }
    }
    
    cout << "订阅者退出" << endl;
    return 0;
}
