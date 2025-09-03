cmake_minimum_required(VERSION 3.10)
project(UniversityPortal C)

# Collect all source files
file(GLOB SOURCES "src/core/*.c" "src/main.c")

# Create executable
add_executable(portal ${SOURCES})

# Add headers directory to search path
target_include_directories(portal PRIVATE ${CMAKE_SOURCE_DIR}/src/headers)
