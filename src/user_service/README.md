# 用户服务 (User Service)

这是一个基于 gRPC 的用户服务，采用 MVC 架构设计，为 gateway 服务提供用户信息数据。

## 架构设计

本服务采用与 gateway 一致的 MVC 架构：

```
src/user_service/
├── controller/           # 控制器层 - 处理 gRPC 请求
│   ├── user_controller.h
│   └── user_controller.cpp
├── service/             # 服务层 - 业务逻辑处理
│   ├── user_service.h/cpp          # 用户业务服务
│   ├── grpc_service_impl.h/cpp     # gRPC 服务实现
│   └── user_client.h/cpp           # 客户端库
├── util/                # 工具层 - 通用工具
│   ├── logger.h/cpp     # 日志工具
│   └── ...
├── conf/                # 配置层 - 配置文件
├── repo/                # 数据访问层 - 数据库操作
├── proto/               # 协议定义
│   └── user.proto       # protobuf 定义文件
├── main.cpp             # 服务主程序
├── test_client.cpp      # 测试客户端
├── build_proto.sh       # protobuf 代码生成脚本
├── start_service.sh     # 服务启动脚本
├── test_system.sh       # 系统测试脚本
├── CMakeLists.txt       # CMake 构建文件
└── README.md            # 说明文档
```

## 功能特性

- 提供用户信息查询服务
- 基于 gRPC 协议，高性能、跨语言
- 采用 MVC 架构，代码结构清晰
- 支持用户ID验证
- 包含完整的错误处理
- 支持健康检查
- 统一的日志系统

## 构建步骤

### 1. 安装依赖

确保已安装以下依赖：
- Protocol Buffers (protoc)
- gRPC C++ 库
- CMake

### 2. 生成 protobuf 代码

```bash
cd src/user_service
./build_proto.sh
```

### 3. 构建服务

```bash
mkdir build
cd build
cmake ..
make
```

## 运行服务

### 启动用户服务

```bash
./user_service_server
```

服务将在 `0.0.0.0:50051` 端口启动。

### 在 Gateway 中使用

在 gateway 服务中，可以通过 `UserClient` 类调用用户服务：

```cpp
#include "user_service/service/user_client.h"

// 创建客户端
user_service::service::UserClient client("localhost:50051");

// 获取用户信息
auto [success, response] = client.GetUserInfo(123);
if (success && response.success()) {
    // 处理用户信息
    auto user_info = response.user_info();
    std::cout << "用户名: " << user_info.username() << std::endl;
}
```

## API 接口

### GetUserInfo

获取用户信息接口。

**请求参数：**
- `user_id` (int32): 用户ID

**响应字段：**
- `success` (bool): 请求是否成功
- `message` (string): 响应消息
- `user_info` (UserInfo): 用户信息对象
- `status_code` (int32): HTTP 状态码
- `timestamp` (int64): 时间戳

**UserInfo 字段：**
- `user_id` (int32): 用户ID
- `username` (string): 用户名
- `email` (string): 邮箱
- `status` (string): 用户状态
- `created_at` (string): 创建时间
- `last_login` (string): 最后登录时间
- `full_name` (string): 全名
- `phone` (string): 电话号码
- `avatar_url` (string): 头像URL

## 错误处理

服务会返回以下错误状态：

- `400`: 无效的用户ID
- `404`: 用户不存在
- `500`: 服务器内部错误

## 测试

可以使用 gRPC 客户端工具（如 grpcurl）测试服务：

```bash
# 安装 grpcurl
go install github.com/fullstorydev/grpcurl/cmd/grpcurl@latest

# 测试服务
grpcurl -plaintext localhost:50051 user.UserService/GetUserInfo -d '{"user_id": 123}'
```

## 配置

服务配置可以通过修改 `main.cpp` 中的 `server_address` 变量来调整监听地址和端口。

## 架构说明

### Controller 层
- 处理 gRPC 请求和响应
- 进行参数验证
- 调用 Service 层处理业务逻辑
- 返回标准化的响应格式

### Service 层
- 实现核心业务逻辑
- 处理数据生成和转换
- 提供客户端库供其他服务调用

### Util 层
- 提供通用工具类
- 统一的日志系统
- 配置管理等功能

### Proto 层
- 定义 gRPC 服务接口
- 定义消息格式
- 支持跨语言调用 