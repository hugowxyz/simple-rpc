set(CMAKE_CXX_COMPILER g++-11)
set(CMAKE_CXX_FLAGS -pthread)

add_executable(ex1 ex1/main.cpp)
add_executable(json-test json-test/main.cpp)

target_link_libraries(ex1 SimpleRPC)