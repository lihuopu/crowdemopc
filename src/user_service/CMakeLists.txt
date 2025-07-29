cmake_minimum_required(VERSION 3.0)
project(user_service)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # 生成编译命令文件，方便IDE解析

# 查找必要的包
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

# find_package(absl REQUIRED)  # 显式查找 Abseil，确保 gRPC 依赖的子库被引入
find_package(absl REQUIRED  # 自动找到 Abseil 所有组件
  COMPONENTS 
  strings         # 包含 StrCat/AsciiStrToLower 等
  synchronization # 包含 Mutex 等
  time            # 包含 Time/FormatTime 等
  status          # 包含 Status/StatusOr 等
  log_severity    # 包含日志相关符号
  base 
  throw_delegate 
  log_internal_check_op  # 核心：包含缺失的ForVar2符号
  log_internal_message 
  log_internal_globals 
  raw_logging_internal 
  spinlock_wait
)

find_package(Threads REQUIRED)  # 线程库（gRPC/Abseil 可能依赖）

# # 批量查找并链接所有 Abseil 静态库
# file(GLOB ABSL_LIBS "/usr/local/lib64/libabsl_*.a")

# 在find_package之后添加
link_directories(/usr/local/lib64)

# 查找 utf8_range 库
find_library(UTF8_RANGE_LIB 
    NAMES utf8_range 
    PATHS 
        /usr/local/lib64 
        /root/utf8_range/build 
        /root/grpc/cmake/build/third_party/protobuf/third_party/utf8_range
)

find_library(UTF8_VALIDITY_LIB 
    NAMES utf8_validity 
    PATHS 
        /usr/local/lib64 
        /root/utf8_range/build 
        /root/grpc/cmake/build/third_party/protobuf/third_party/utf8_range
)

# 检查是否找到库
if(NOT UTF8_RANGE_LIB OR NOT UTF8_VALIDITY_LIB)
    message(FATAL_ERROR "utf8_range 或 utf8_validity 库未找到，请检查安装路径")
else()
    message(STATUS "找到 utf8_range 库: ${UTF8_RANGE_LIB}")
    message(STATUS "找到 utf8_validity 库: ${UTF8_VALIDITY_LIB}")
endif()

# 查找 cpprest 动态库（重点：添加 lib64 路径）
find_library(CPPREST_LIB 
    NAMES cpprest  # 库名（对应 libcpprest.so）
    PATHS 
        /usr/local/lib64  # 你的 libcpprest.so 所在路径
        /root/cpprestsdk/build/Release/Binaries  # 编译目录的备份路径
)
# 检查是否找到库
if(NOT CPPREST_LIB)
    message(FATAL_ERROR "未找到 libcpprest.so，请确认路径是否正确")
else()
    message(STATUS "找到 cpprest 动态库: ${CPPREST_LIB}")  # 应输出 /usr/local/lib64/libcpprest.so
endif()

# 禁用 CMake 自带的 Boost 查找模块，但允许搜索系统路径（因为已安装到 /usr/local）
set(Boost_NO_BOOST_CMAKE ON)
# 注释掉 Boost_NO_SYSTEM_PATHS，允许 CMake 搜索系统默认路径（如 /usr/local）
# set(Boost_NO_SYSTEM_PATHS ON)

# 实际 Boost 1.66.0 安装到了 /usr/local（系统默认路径）
set(BOOST_ROOT "/usr/local")  # 头文件在 /usr/local/include，库在 /usr/local/lib
set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")  # 头文件目录
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib64")    # 库文件目录（64位系统通常在 lib64）

# 查找 Boost 1.66.0 及所需组件
find_package(Boost 1.66.0 REQUIRED COMPONENTS system thread)

if(Boost_FOUND)
    message(STATUS "找到 Boost 1.66.0 头文件: ${Boost_INCLUDE_DIRS}")
    message(STATUS "找到 Boost 1.66.0 库: ${Boost_LIBRARIES}")
    include_directories(${Boost_INCLUDE_DIRS})  # 确保头文件可被引用
else()
    message(FATAL_ERROR "未找到 Boost 1.66.0，请确认已安装到 /usr/local")
endif()
# 查找 etcd-cpp-api 库
find_library(ETCD_API_LIB 
    NAMES etcd-cpp-api 
    PATHS 
        /usr/local/lib64  # 系统安装路径
        /root/etcd-cpp-apiv3/build/src  # 编译目录（备用）
)

if(NOT ETCD_API_LIB)
    message(FATAL_ERROR "未找到 etcd-cpp-api 库，请检查安装")
else()
    message(STATUS "找到 etcd-cpp-api 库: ${ETCD_API_LIB}")
endif()

# 定义所有目标共享的库（统一管理，避免重复）
set(COMMON_LIBS
    ${Protobuf_LIBRARIES}
    gRPC::grpc++
    gRPC::grpc++_reflection  # 服务器需要的反射库
    Threads::Threads  # 线程库（解决可能的 pthread 依赖）
    stdc++fs          # 文件系统库（若代码中使用了 C++17 filesystem）
    
    
    ${YAML_CPP_LIBRARY}  # 若依赖 yaml-cpp，补充进来
    # UTF-8 验证库
    ${UTF8_RANGE_LIB}
    ${UTF8_VALIDITY_LIB}
    # ${ABSL_LIBS}  # 批量引入所有 Abseil 库
    # Abseil 核心库（根据错误补充所需组件）
    absl::strings          # 解决 StrCat/AsciiStrToLower 等
    absl::synchronization  # 解决 Mutex 等
    absl::time             # 解决 Now()/FormatTime 等
    absl::status           # 解决 Status/UnauthenticatedError 等
    absl::log_severity     # 解决日志相关符号
    absl::base             # 基础组件
    absl::throw_delegate   # 异常处理相关


    # 新增：日志内部组件（解决 LogMessageFatal 问题）
    absl::log_internal_message    # 核心：提供 LogMessageFatal
    absl::log_internal_globals    # 日志全局状态
    absl::log_internal_check_op   # 日志检查操作依赖
    absl::raw_logging_internal
    absl::spinlock_wait
)

# # 包含目录
# include_directories(${CMAKE_CURRENT_BINARY_DIR})
# include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# include_directories(${Protobuf_INCLUDE_DIRS})
# include_directories(${gRPC_INCLUDE_DIRS})

# 包含目录
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Protobuf_INCLUDE_DIRS}
    ${gRPC_INCLUDE_DIRS}
    # 若依赖上层项目的头文件，补充路径
    # 示例：若用到 gateway 的工具类
    /root/etcd-cpp-apiv3/include
    
)

# 用户服务服务器
add_executable(user_service_server
    main.cpp
    controller/user_controller.cpp
    controller/user_controller.h
    service/user_service.cpp
    service/user_service.h
    service/grpc_service_impl.cpp
    service/grpc_service_impl.h
    service/user_client.cpp
    service/user_client.h
    util/logger.cpp
    util/logger.h
    # 手动生成的Protobuf文件
    proto/user.pb.cc
    proto/user.pb.h
    proto/user.grpc.pb.cc
    proto/user.grpc.pb.h
)

# 用户服务客户端库
add_library(user_service_client
    service/user_client.cpp
    service/user_client.h
    util/logger.cpp
    util/logger.h
    # 手动生成的Protobuf文件
    proto/user.pb.cc
    proto/user.pb.h
    proto/user.grpc.pb.cc
    proto/user.grpc.pb.h
)

# 测试客户端
add_executable(user_service_test_client
    test_client.cpp
    service/user_client.cpp
    service/user_client.h
    util/logger.cpp
    util/logger.h
    # 手动生成的Protobuf文件
    proto/user.pb.cc
    proto/user.pb.h
    proto/user.grpc.pb.cc
    proto/user.grpc.pb.h
)

# 链接库
target_link_libraries(user_service_server
    ${Protobuf_LIBRARIES}
    gRPC::grpc++
    gRPC::grpc++_reflection
    ${CPPREST_LIB}       # 新增：cpprest 动态库
    ${Boost_LIBRARIES}   # 新增：boost 依赖库
    ${ETCD_API_LIB}  # 确保 etcd 库在链接列表中
    ${COMMON_LIBS}
)

target_link_libraries(user_service_client
    ${COMMON_LIBS}
)

target_link_libraries(user_service_test_client
    ${COMMON_LIBS}
)

# 编译选项
# target_compile_options(user_service_server PRIVATE -Wall -Wextra)
# target_compile_options(user_service_client PRIVATE -Wall -Wextra)
# target_compile_options(user_service_test_client PRIVATE -Wall -Wextra) 
# 编译选项（与上层项目保持一致）
target_compile_options(user_service_server PRIVATE
    -Wall -Wextra
    -Wno-unused-parameter  # 可选：抑制未使用参数的警告
)
target_compile_options(user_service_client PRIVATE ${rpc_server_COMPILE_OPTIONS})
target_compile_options(user_service_test_client PRIVATE ${rpc_server_COMPILE_OPTIONS})