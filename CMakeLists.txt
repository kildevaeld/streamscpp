cmake_minimum_required(VERSION 3.1)
project(streams++)

set(CMAKE_CXX_STANDARD 14)

find_package(libarchive)



add_subdirectory(src)
add_subdirectory(example)

# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()