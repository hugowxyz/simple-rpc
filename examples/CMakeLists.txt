set(CMAKE_CXX_COMPILER g++-11)
set(CMAKE_CXX_FLAGS -pthread)

add_executable(ex1 ex1/main.cpp)
add_executable(server-benchmarker-client server-benchmarker-client/main.cpp)
add_executable(rpc-node rpc-node/main.cpp)
add_executable(server-benchmark server-benchmark/main.cpp)
add_executable(what what/main.cpp)

target_link_libraries(ex1 SimpleRPC)
target_link_libraries(rpc-node SimpleRPC)
target_link_libraries(server-benchmarker-client SimpleRPC)
target_link_libraries(server-benchmark SimpleRPC)