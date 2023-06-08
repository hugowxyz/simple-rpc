cmake_minimum_required(VERSION 3.0.0)
project(SimpleRPC)

set(CMAKE_CXX_COMPILER g++-11)
set(CMAKE_CXX_STANDARD 20)

include_directories(include dependencies/include)
add_subdirectory(examples)
add_subdirectory(src)
