cmake_minimum_required(VERSION 3.10)
project(distributed_integration)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(master master.cpp)
add_executable(worker worker.cpp)

target_compile_options(master PRIVATE -Wall -Wextra)
target_compile_options(worker PRIVATE -Wall -Wextra)

target_link_libraries(master pthread)
