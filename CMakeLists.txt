cmake_minimum_required(VERSION 3.10)
project(UniversityPortal C)

file(GLOB SOURCES "src/core/*.c" "src/main.c")

add_executable(portal ${SOURCES})

target_include_directories(portal PRIVATE ${CMAKE_SOURCE_DIR}/src/headers)
