#!/bin/bash

# 系统测试脚本

echo "=== 用户服务系统测试 ==="

# 检查用户服务是否运行
if ! pgrep -f "user_service_server" > /dev/null; then
    echo "错误: 用户服务未运行，请先启动用户服务"
    echo "运行命令: ./start_service.sh"
    exit 1
fi

echo "✓ 用户服务正在运行"

# 测试用户服务客户端
echo "测试用户服务客户端..."
cd build
if [ -f "user_service_test_client" ]; then
    ./user_service_test_client
    if [ $? -eq 0 ]; then
        echo "✓ 用户服务客户端测试通过"
    else
        echo "✗ 用户服务客户端测试失败"
        exit 1
    fi
else
    echo "错误: 测试客户端未找到，请先构建项目"
    exit 1
fi

cd ..

echo ""
echo "=== 测试完成 ==="
echo ""
echo "现在可以测试 gateway 服务："
echo "1. 启动 gateway 服务"
echo "2. 访问 http://localhost:8080/api/user/123"
echo "3. 检查返回的用户信息是否来自用户服务" 