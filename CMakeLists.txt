project(webserver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

link_libraries(libmysql)
include_directories(/usr/include/mysql/)

# 设置 include Path
include_directories(${CMAKE_SOURCE_DIR}/include)

# 设置源文件(需要编译的文件) 目录
aux_source_directory(./src/ DIR_SRCS)

add_executable(webserver ${DIR_SRCS} include/sqlconnpool.h include/sqlconnRAII.h src/http/httpconn.h src/http/httprequest.h src/http/httpresponse.h src/http/httprequest.cpp src/http/httpresponse.cpp src/http/httpconn.cpp src/server/webserver.h src/server/webserver.cpp)

target_include_directories( webserver
        PRIVATE ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(webserver pthread)