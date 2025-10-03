cmake_minimum_required(VERSION 3.10)
project(v2_cli LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Biblioteca de logging
add_library(libtslog STATIC src/libtslog.cpp)
target_include_directories(libtslog PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Servidor
add_executable(server src/server.cpp)
target_link_libraries(server PRIVATE libtslog pthread)

# Cliente
add_executable(client src/client.cpp)
target_link_libraries(client PRIVATE libtslog pthread)

# Teste CLI
add_executable(test_cli tests/test_cli.cpp)
target_link_libraries(test_cli PRIVATE libtslog pthread)
