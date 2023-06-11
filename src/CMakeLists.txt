set(OUTPUT_LIBRARY_NAME ${CMAKE_PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)

add_library(${CMAKE_PROJECT_NAME}
        simple-rpc/server.cpp
        simple-rpc/tcp_connection.cpp
        simple-rpc/client.cpp
        ../include/simple-rpc/utils.h)

target_link_libraries(${OUTPUT_LIBRARY_NAME})