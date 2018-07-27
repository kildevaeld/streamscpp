
set(SOURCES
    file-streams.cpp
    memory-streams.cpp
    http-streams.cpp
    utils.cpp
    package.cpp
    chain.cpp

    #producers
    producers/vector.cpp
    producers/directory.cpp
    # Transformers
    transformers/directory.cpp

    # private
    downloader.cpp
)

set(LIBS curl workq++ ${Boost_LIBRARIES} syrup-path)

if (LibArchive_FOUND)
    message(STATUS "Adding archive-streams")
    set(SOURCES ${SOURCES}  archive.cpp)
    set(LIBS ${LIBS} ${LibArchive_LIBRARIES})
endif(LibArchive_FOUND)

add_library(streams++ ${SOURCES})
target_link_libraries(streams++ ${LIBS})

target_include_directories(streams++ PUBLIC ${PROJECT_SOURCE_DIR}/include)