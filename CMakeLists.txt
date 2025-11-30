cmake_minimum_required(VERSION 3.12)

project(NDM_Test_1 CXX)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

include_directories(
    src
)

add_library(udp_socket STATIC
src/udp/udp_socket.cpp
)

add_library(tcp_socket STATIC
src/tcp/tcp_socket.cpp
)

add_library(epoll STATIC
    src/epoll/epoll.cpp
)

add_library(handler STATIC
    src/handler/handler.cpp
)

set(EXECUTABLE_NAME main_NDM)

add_executable(${EXECUTABLE_NAME} main.cpp)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE udp_socket tcp_socket epoll handler)

# Deb pack

set(DEB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deb_pack)
set(BIN_DIR ${DEB_DIR}/usr/local/bin)
file(MAKE_DIRECTORY ${BIN_DIR})

add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${EXECUTABLE_NAME}> ${BIN_DIR}
)

add_custom_target(package_deb ALL
    COMMAND dpkg-deb --build ${DEB_DIR} ${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE_NAME}.deb
    DEPENDS ${EXECUTABLE_NAME}
)