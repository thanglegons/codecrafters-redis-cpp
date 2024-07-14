cmake_minimum_required(VERSION 3.13)

project(redis-starter-cpp)

include_directories(${PROJECT_SOURCE_DIR}/src)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(asio CONFIG REQUIRED)

add_executable(server ${SOURCE_FILES})

target_link_libraries(server PRIVATE asio asio::asio)
target_link_libraries(server PRIVATE Threads::Threads)

find_package(GTest CONFIG REQUIRED)
enable_testing()
add_executable(server_test ${PROJECT_SOURCE_DIR}/tests/main.cpp)
target_link_libraries(server_test PRIVATE GTest::gtest GTest::gtest_main)
add_test(AllTestsInMain server_test)