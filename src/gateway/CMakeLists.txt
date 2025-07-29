cmake_minimum_required(VERSION 3.0)

project(crow_demo VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 主项目 CMakeLists.txt 开头添加
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

# 设置asio路径
set(ASIO_INCLUDE_DIR "/thirdparty/asio-1.30.2/include/")

# 查找asio
find_path(ASIO_INCLUDE_DIR asio.hpp
    PATHS
    /thirdparty/asio-1.30.2/include
)

if(NOT ASIO_INCLUDE_DIR)
    message(FATAL_ERROR "asio not found")
endif()

# 查找yaml-cpp
find_path(YAML_CPP_INCLUDE_DIR yaml-cpp/yaml.h
    PATHS
    /usr/include
    /usr/local/include
)

find_library(YAML_CPP_LIBRARY
    NAMES yaml-cpp
    PATHS
    /usr/lib64
    /usr/lib
    /usr/local/lib64
    /usr/local/lib
)

if(NOT YAML_CPP_INCLUDE_DIR OR NOT YAML_CPP_LIBRARY)
    message(FATAL_ERROR "yaml-cpp not found")
endif()

message(STATUS "Found yaml-cpp: ${YAML_CPP_LIBRARY}")

# # 查找gRPC和Protobuf
# find_package(Protobuf REQUIRED)
# find_package(gRPC REQUIRED)
# 查找 Abseil 库（如果 gRPC 未自动引入）
find_package(absl REQUIRED)
# 定义所有源文件
set(SOURCES
    main.cpp
    util/response_util.cpp
    util/request_util.cpp
    util/logger.cpp
    util/config.cpp
    service/health_service.cpp
    service/hello_service.cpp
    service/calculator_service.cpp
    service/api_service.cpp
    controller/health_controller.cpp
    controller/hello_controller.cpp
    controller/calculator_controller.cpp
    controller/api_controller.cpp
    controller/websocket_controller.cpp
    # 用户服务客户端
    ../../user_service/service/user_client.cpp
    ../../user_service/util/logger.cpp
    # 手动生成的Protobuf文件
    ../../user_service/proto/user.pb.cc
    ../../user_service/proto/user.grpc.pb.cc
)

# 查找 utf8_range 静态库（指定已知路径）
find_library(UTF8_RANGE_LIBRARY
    NAMES utf8_range  # 库名（实际文件是 libutf8_range.a）
    PATHS 
        /usr/local/lib64  # 优先使用系统级安装的库
        /root/grpc/cmake/build/third_party/protobuf/third_party/utf8_range  # 备选路径
)

# 检查是否找到库
if(NOT UTF8_RANGE_LIBRARY)
    message(FATAL_ERROR "utf8_range library not found. Please check the paths.")
endif()

# 查找 utf8_validity 库（包含 IsStructurallyValid 符号）
find_library(UTF8_VALIDITY_LIBRARY
    NAMES utf8_validity
    PATHS /usr/local/lib64
)

# 确保找到库
if(NOT UTF8_VALIDITY_LIBRARY)
    message(FATAL_ERROR "libutf8_validity.a not found")
endif()

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${ASIO_INCLUDE_DIR}
    ${YAML_CPP_INCLUDE_DIR}
    ${Protobuf_INCLUDE_DIRS}
    ${gRPC_INCLUDE_DIRS}
    /thirdparty/Crow-1.2.1-Linux/include
    ./
    ../../user_service
    ../../user_service/proto
)

# 添加必要的编译选项
target_compile_options(${PROJECT_NAME} PRIVATE
    -DASIO_STANDALONE
    -DASIO_NO_DEPRECATED
)

# 链接pthread库、filesystem库、yaml-cpp库、protobuf库和gRPC库
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE 
    Threads::Threads
    stdc++fs
    ${YAML_CPP_LIBRARY}
    ${Protobuf_LIBRARIES}
    gRPC::grpc++
    # 补充 Abseil 相关库（解决日志符号缺失问题）
    absl::base
    absl::log
    absl::log_internal_check_op
    absl::log_internal_conditions
    absl::log_internal_message
    absl::log_internal_nullguard
    absl::log_sink

    # # 新增：链接 utf8_range 库
    # utf8_range::utf8_range  # 若 CMake 能识别目标，优先使用此方式

     # 新增：链接 utf8_validity
     ${UTF8_VALIDITY_LIBRARY}
      # 已有的其他库（Threads、yaml-cpp、protobuf、grpc、absl 等）
    ${UTF8_RANGE_LIBRARY}  # 新增：链接 utf8_range 静态库
)
