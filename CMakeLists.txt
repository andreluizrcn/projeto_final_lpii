cmake_minimum_required(VERSION 3.5)
project(v1_logging LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(${CMAKE_SOURCE_DIR}/include)


add_library(libtslog STATIC src/libtslog.cpp)
add_executable(test_cli tests/test_cli.cpp)
target_link_libraries(test_cli PRIVATE libtslog)


install(TARGETS libtslog test_cli DESTINATION bin)
