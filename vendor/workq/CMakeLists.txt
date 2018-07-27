cmake_minimum_required(VERSION 3.11)
project(workq++)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED 11)

option(WORKQ_DEBUG "enable debugging output" off)

#include_directories(${PROJECT_SOURCE_DIR}/vendor/spdlog/include)
add_subdirectory(src)
#add_subdirectory(example)