cmake_minimum_required(VERSION 3.5)

project(chess)

INCLUDE(CMakeLists.clean.txt)

# Abseil requires C++11
set(CMAKE_CXX_STANDARD 11)

set(SRCS
    main.cpp
)

add_executable(chess ${SRCS})

target_link_libraries(chess)
