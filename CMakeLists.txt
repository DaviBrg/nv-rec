cmake_minimum_required(VERSION 2.8)

project(nvrec)

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC TRUE)

set(HEADERS
    include/ycsb.pb.h
    include/ycsb.grpc.pb.h
)

set(SOURCES
    src/main.cc
    src/ycsb.pb.cc
    src/ycsb.grpc.pb.cc
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} -lpmemobj -lpmemlog -lpthread -lprotobuf -lgrpc++)


target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)
