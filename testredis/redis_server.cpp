#include "redis.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

//g++ redis_server.cpp redis.cpp -o redis_server -lhiredis -lpthread
using namespace std;

int main() {
    // 创建Redis对象并连接服务器
    Redis redis;
    if (!redis.connect()) {
        cerr << "连接Redis服务器失败!" << endl;
        return 1;
    }

    cout << "Redis发布者已启动，输入消息发送到通道(输入quit退出)..." << endl;
    
    int channel = 100; // 发布消息的通道
    string message;
    
    while (true) {
        cout << "请输入要发送的消息: ";
        getline(cin, message);
        
        if (message == "quit") {
            break;
        }
        
        // 向指定通道发布消息
        if (redis.publish(channel, message)) {
            cout << "消息发布成功: " << message << endl;
        } else {
            cerr << "消息发布失败!" << endl;
        }
        
        // 简单延时，避免CPU占用过高
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    
    cout << "发布者退出" << endl;
    return 0;
}
