cmake_minimum_required(VERSION 3.10)

project(TCP_Framework VERSION 1.5)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Enable warnings
add_compile_options(-Wall -Wextra -g)

# Include header directory
include_directories(include)

# -------------------------
# Server Executable
# -------------------------
add_executable(server
    src/main_server.cpp
    src/Server.cpp
    src/ThreadPool.cpp
)

target_link_libraries(server pthread)

# -------------------------
# Client Executable
# -------------------------
add_executable(client
    src/main_client.cpp
)

target_link_libraries(client pthread)
