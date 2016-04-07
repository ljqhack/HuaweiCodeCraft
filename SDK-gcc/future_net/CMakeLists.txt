# CMake 最低版本号要求
cmake_minimum_required(VERSION 2.8)

# 项目信息
project(future_net)

# include路径
include_directories(${PROJECT_SOURCE_DIR}/lib)
include_directories(${PROJECT_SOURCE_DIR}/header)

# 设置可执行文件生成路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin)

# 生成debug版本
SET(CMAKE_BUILD_TYPE "Debug") 
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")


# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(. DIR_SRCS)

# 指定生成目标 
add_executable(future_net ${DIR_SRCS})


# 生成带有路径的连接库，存储于 FUTURE_NET_LIBRARY 变量
find_library(MY_LIBRARY1 hello ${PROJECT_BINARY_DIR}/../future_net/)
IF ( MY_LIBRARY1)
    #导入静态库
    TARGET_LINK_LIBRARIES(future_net ${MY_LIBRARY1})
    #MESSAGE(FATAL_ERROR "${MY_LIBRARY1} , path:${PROJECT_BINARY_DIR} ")
ENDIF( MY_LIBRARY1)

# 生成带有路径的连接库，存储于 FUTURE_NET_LIBRARY 变量
find_library(MY_LIBRARY2 glpk ${PROJECT_BINARY_DIR}/../future_net/)
IF ( MY_LIBRARY2)
    #导入静态库
    TARGET_LINK_LIBRARIES(future_net ${MY_LIBRARY2})
    #MESSAGE(FATAL_ERROR "${MY_LIBRARY2} , path:${PROJECT_BINARY_DIR} ")
ENDIF( MY_LIBRARY2)
 
#link_directories(${PROJECT_SOURCE_DIR})
#target_link_libraries(future_net ${PROJECT_SOURCE_DIR}/lib/libhello.a)



