#!/bin/bash

# 检查 protoc 是否安装
if ! command -v protoc &> /dev/null; then
    echo "错误: protoc 未安装，请先安装 Protocol Buffers"
    exit 1
fi

# 检查 grpc_cpp_plugin 是否安装
if ! command -v grpc_cpp_plugin &> /dev/null; then
    echo "错误: grpc_cpp_plugin 未安装，请先安装 gRPC"
    exit 1
fi

# 创建 proto 目录（如果不存在）
mkdir -p proto

# 生成 C++ 代码
echo "正在生成 protobuf C++ 代码..."
protoc --cpp_out=. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` proto/user.proto

if [ $? -eq 0 ]; then
    echo "protobuf 代码生成成功！"
    echo "生成的文件:"
    echo "  - user.pb.h"
    echo "  - user.pb.cc"
    echo "  - user.grpc.pb.h"
    echo "  - user.grpc.pb.cc"
else
    echo "错误: protobuf 代码生成失败"
    exit 1
fi 