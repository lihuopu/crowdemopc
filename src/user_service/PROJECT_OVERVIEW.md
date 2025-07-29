# 用户服务系统总览

## 系统架构

本系统采用微服务架构，包含以下组件：

### 1. 用户服务 (User Service)
- **位置**: `src/user_service/`
- **类型**: gRPC 服务
- **端口**: 50051
- **功能**: 提供用户信息查询服务

### 2. Gateway 服务
- **位置**: `src/gateway/`
- **类型**: HTTP REST API 服务
- **端口**: 8080 (可配置)
- **功能**: 对外提供 HTTP 接口，内部调用用户服务

## 系统流程

```
客户端请求 → Gateway (HTTP) → 用户服务 (gRPC) → 返回用户信息
```

1. 客户端向 Gateway 发送 HTTP 请求：`GET /api/user/{user_id}`
2. Gateway 验证请求并调用用户服务的 gRPC 接口
3. 用户服务返回用户信息
4. Gateway 将用户信息转换为 JSON 格式返回给客户端

## 文件结构

```
src/
├── user_service/           # 用户服务
│   ├── proto/
│   │   └── user.proto     # protobuf 定义
│   ├── user_service_impl.h/cpp    # 服务实现
│   ├── user_client.h/cpp          # 客户端库
│   ├── main.cpp                   # 服务主程序
│   ├── test_client.cpp            # 测试客户端
│   ├── build_proto.sh             # protobuf 生成脚本
│   ├── start_service.sh           # 服务启动脚本
│   ├── test_system.sh             # 系统测试脚本
│   ├── CMakeLists.txt             # 构建配置
│   └── README.md                  # 详细文档
└── gateway/               # Gateway 服务
    ├── service/
    │   └── api_service.h/cpp      # 已修改，支持调用用户服务
    └── ...
```

## 快速开始

### 1. 启动用户服务

```bash
cd src/user_service
./start_service.sh
```

### 2. 测试用户服务

```bash
./test_system.sh
```

### 3. 启动 Gateway 服务

```bash
cd src/gateway
# 按照原有方式启动 gateway 服务
```

### 4. 测试完整系统

```bash
# 测试用户信息接口
curl http://localhost:8080/api/user/123
```

## 特性

### 容错机制
- 如果用户服务不可用，Gateway 会自动回退到模拟数据
- 支持服务健康检查
- 包含完整的错误处理

### 性能优化
- 使用 gRPC 进行服务间通信，性能优异
- 支持连接池和超时控制
- 异步处理能力

### 可扩展性
- 微服务架构，易于扩展
- 支持服务发现和负载均衡
- 可以轻松添加新的用户相关功能

## 配置说明

### 用户服务配置
- 监听地址：`0.0.0.0:50051`
- 可在 `main.cpp` 中修改

### Gateway 配置
- 用户服务地址：`localhost:50051`
- 可在 `api_service.cpp` 中修改

## 开发指南

### 添加新的用户功能

1. 在 `proto/user.proto` 中添加新的消息和服务定义
2. 运行 `./build_proto.sh` 生成新的代码
3. 在 `user_service_impl.cpp` 中实现新的服务方法
4. 在 `user_client.cpp` 中添加客户端调用方法
5. 在 Gateway 中添加相应的 HTTP 接口

### 调试技巧

1. 使用 `grpcurl` 工具直接测试 gRPC 服务
2. 查看服务日志了解调用情况
3. 使用 `test_client.cpp` 进行单元测试

## 部署说明

### 生产环境部署

1. 确保所有依赖已安装（gRPC、Protobuf、CMake）
2. 构建所有服务
3. 配置服务发现和负载均衡
4. 设置监控和日志收集
5. 配置安全策略（TLS、认证等）

### Docker 部署

可以创建 Docker 镜像来简化部署：

```dockerfile
# 用户服务 Dockerfile 示例
FROM ubuntu:20.04
# ... 安装依赖和构建步骤
```

## 故障排除

### 常见问题

1. **用户服务启动失败**
   - 检查端口 50051 是否被占用
   - 确认 gRPC 依赖已正确安装

2. **Gateway 无法连接用户服务**
   - 确认用户服务正在运行
   - 检查网络连接和防火墙设置

3. **protobuf 编译错误**
   - 确认 protoc 和 grpc_cpp_plugin 已安装
   - 检查 proto 文件语法

### 日志查看

- 用户服务日志：直接输出到控制台
- Gateway 日志：查看 gateway 的日志配置

## 性能监控

建议监控以下指标：
- 服务响应时间
- 错误率
- 并发连接数
- 资源使用情况

## 安全考虑

- 在生产环境中启用 TLS
- 实现适当的认证和授权
- 限制服务访问权限
- 定期更新依赖库 