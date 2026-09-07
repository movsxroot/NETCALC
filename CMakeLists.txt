cmake_minimum_required(VERSION 4.3)
project(NetCalc C)

set(CMAKE_C_STANDARD 11)

add_executable(untitled1 ip.c
        ip.h
)

target_link_libraries(untitled1 PRIVATE m)
