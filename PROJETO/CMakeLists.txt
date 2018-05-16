cmake_minimum_required(VERSION 3.9)
project(clipboard C)

set(CMAKE_C_STANDARD 99)
SET(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-Wall -pthread")

#Server
add_executable(clipboard clipboard.c socket_lib.c)
#Client
#add_executable(client app_teste.c library.c socket_lib.c )