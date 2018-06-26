
set(SOURCES
    file-streams.cpp
    memory-streams.cpp
    utils.cpp
)

add_library(streams++ ${SOURCES})
target_include_directories(streams++ PUBLIC ${PROJECT_SOURCE_DIR}/include)