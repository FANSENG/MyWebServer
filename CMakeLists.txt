project(webserver)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SRC_LIST
    ./src/main.cc
)

add_executable(webserver ${SRC_LIST})

# target_link_libraries(webserver pthread mysqlclient)
