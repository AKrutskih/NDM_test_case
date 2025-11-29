cmake_minimum_required(VERSION 3.12)

project(NDM_Test_1 CXX)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

include_directories(
    src/tcp
    src/udp
)

# add_library(server STATIC
#     src/server.cpp
# )

add_library(udp_socket STATIC
    src/udp/udp_socket.cpp
)

add_library(tcp_socket STATIC
    src/tcp/tcp_socket.cpp
)

add_executable(main_NDM main.cpp)
target_link_libraries(main_NDM PRIVATE udp_socket tcp_socket)