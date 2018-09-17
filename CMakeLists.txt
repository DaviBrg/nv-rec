cmake_minimum_required(VERSION 2.8)

project(nv-rec)

set (CMAKE_CXX_STANDARD 17)

set(HEADERS
    include/logger.h
)

set(SOURCES
    src/logger.cpp
    src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} -lpmemobj -lpmemlog -lpthread)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)
