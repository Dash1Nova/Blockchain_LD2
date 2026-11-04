cmake_minimum_required(VERSION 3.14)
project(LD2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(LD2 src/LD2.cpp src/hash.cpp src/merkle.cpp src/block.cpp)
target_include_directories(LD2 PRIVATE src)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(tests src/tests.cpp src/hash.cpp src/merkle.cpp src/block.cpp)
target_include_directories(tests PRIVATE src)
target_link_libraries(tests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)