# minimun version of cmake  required 
cmake_minimum_required(VERSION 3.10)

# Name of the Projects
project(tcpclient)

# What version of C++ is required to run the projects
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# we define some flags with detect the system and set the flag accordinly

if(WIN32)
    set(CLIENT_OS_SRC win/win.cpp)
elseif(UNIX AND NOT APPLE)
    set(CLIENT_OS_SRC lin/lin.cpp)
elseif(APPLE)
    set(CLIENT_OS_SRC mac/mac.cpp)
endif()

# add library
add_library(tcpclient STATIC tcpclient.cpp ${CLIENT_OS_SRC})

target_include_directories(tcpclient PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})



