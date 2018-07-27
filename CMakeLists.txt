cmake_minimum_required(VERSION 3.1)
project(streams++)

set(CMAKE_CXX_STANDARD 14)

find_package(libarchive)

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(Boost 1.36.0 COMPONENTS filesystem)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

add_subdirectory(vendor/syrup-path)
add_subdirectory(vendor/workq)
add_subdirectory(src)
add_subdirectory(example)

# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()