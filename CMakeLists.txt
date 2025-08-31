# minimun version of cmake  required 
cmake_minimum_required(VERSION 3.10)

# Name of the Projects
project(HTTP_SERVER_1_0V)

# What version of C++ is required to run the projects
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# we define some flags with detect the system and set the flag accordinly

if(WIN32)
    set(PLATFORM_NAME "windows")
    add_definitions(-DPLATFORM_WINDOWS)
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_NAME "linux")
    add_definitions(-DPLATFORM_LINUX)
elseif(APPLE)
    set(PLATFORM_NAME "mac")
    add_definitions(-DPLATFORM_MAC)
endif()

add_subdirectory(tcpserver)
add_subdirectory(tcpclient)
add_subdirectory(httpserver)
add_subdirectory(examples)

# mkdir build
# cd build
# cmake ..
# cmake --build .
# cd examples/Debug/server.exe
# cd examples/Debug/client.exe




