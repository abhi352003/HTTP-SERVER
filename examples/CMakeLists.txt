project(demo)


# server build for test
add_executable(server server.cpp)
target_link_libraries(server tcpserver)
set(INCLUDE_SERVER_DIRS "../tcpserver")
target_include_directories(server PUBLIC ${INCLUDE_SERVER_DIRS})


# client build for test
add_executable(client client.cpp)
target_link_libraries(client tcpclient)
set(INCLUDE_CLIENT_DIRS "../tcpclient")
target_include_directories(client PUBLIC ${INCLUDE_CLIENT_DIRS})






