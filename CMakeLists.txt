cmake_minimum_required(VERSION 2.8)

project(nvrec)

set (CMAKE_CXX_STANDARD 17)

set(HEADERS
    include/logger.h
)

set(SOURCES
    src/logger.cpp
    src/main.cpp
)

add_executable(nvrec-server ${SOURCES} ${HEADERS})

target_link_libraries(nvrec-server -lpmemobj -lpmemlog -lpthread -lboost_thread
    )

target_include_directories(nvrec-server
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)
