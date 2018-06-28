
set(SOURCES
    file-streams.cpp
    memory-streams.cpp
    utils.cpp
)

set(LIBS)

if (LibArchive_FOUND)
    message(STATUS "Adding archive-streams")
    set(SOURCES ${SOURCES}  archive.cpp archive-streams.cpp)
    set(LIBS ${LIBS} ${LibArchive_LIBRARIES})
endif(LibArchive_FOUND)

add_library(streams++ ${SOURCES})
target_link_libraries(streams++ ${LIBS})

target_include_directories(streams++ PUBLIC ${PROJECT_SOURCE_DIR}/include)