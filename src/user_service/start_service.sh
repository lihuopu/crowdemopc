#!/bin/bash

# 用户服务启动脚本

echo "=== 用户服务启动脚本 ==="

# 检查是否在正确的目录
if [ ! -f "main.cpp" ]; then
    echo "错误: 请在 src/user_service 目录下运行此脚本"
    exit 1
fi

# 检查构建目录
if [ ! -d "build" ]; then
    echo "创建构建目录..."
    mkdir -p build
fi

# 检查是否需要生成 protobuf 文件
if [ ! -f "user.pb.h" ] || [ ! -f "user.grpc.pb.h" ]; then
    echo "生成 protobuf 文件..."
    ./build_proto.sh
    if [ $? -ne 0 ]; then
        echo "错误: protobuf 文件生成失败"
        exit 1
    fi
fi

# 进入构建目录
cd build

# 检查是否需要重新构建
if [ ! -f "user_service_server" ] || [ ! -f "user_service_test_client" ]; then
    echo "构建用户服务..."
    cmake ..
    if [ $? -ne 0 ]; then
        echo "错误: CMake 配置失败"
        exit 1
    fi
    
    make
    if [ $? -ne 0 ]; then
        echo "错误: 编译失败"
        exit 1
    fi
fi

echo "用户服务构建完成"

# 检查服务是否已经在运行
if pgrep -f "user_service_server" > /dev/null; then
    echo "警告: 用户服务已经在运行"
    echo "PID: $(pgrep -f user_service_server)"
else
    echo "启动用户服务..."
    ./user_service_server &
    SERVER_PID=$!
    echo "用户服务已启动，PID: $SERVER_PID"
    
    # 等待服务启动
    sleep 2
    
    # 检查服务是否成功启动
    if kill -0 $SERVER_PID 2>/dev/null; then
        echo "✓ 用户服务启动成功，监听端口 50051"
        echo "按 Ctrl+C 停止服务"
        
        # 等待用户中断
        trap "echo '正在停止服务...'; kill $SERVER_PID; exit 0" INT TERM
        wait $SERVER_PID
    else
        echo "✗ 用户服务启动失败"
        exit 1
    fi
fi 