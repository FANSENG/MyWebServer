cmake_minimum_required(VERSION 3.16)
project(webserver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(/usr/include/mysql/)

# 设置 include Path
include_directories(${CMAKE_SOURCE_DIR}/src/include)

# 设置源文件(需要编译的文件) 目录
# aux_source_directory(src/ DIR_SRCS)

set(SRCS
        src/buffer/buffer.cc
        src/http/httpconn.cc
        src/http/httprequest.cc
        src/http/httpresponse.cc
        src/log/log.cc
        src/server/epoller.cc
        src/server/webserver.cc
        src/sql/sqlconnpool.cc
        src/timer/time.cc
        src/main.cc
)

add_executable(webserver ${SRCS})



target_link_libraries(webserver pthread mysqlclient)

# target_include_directories(webserver PRIVATE ${PROJECT_SOURCE_DIR}/src/include)