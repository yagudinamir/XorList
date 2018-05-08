cmake_minimum_required(VERSION 3.8)
project(Allocator)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(googletest-master-2)
include_directories(googletest-master-2/googletest/include)
set(SOURCE_FILES main.cpp XorList.h StackAllocator.h GoogleTests.h)
add_executable(Allocator ${SOURCE_FILES})
target_link_libraries(Allocator gtest gtest_main)
