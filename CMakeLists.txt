cmake_minimum_required(VERSION 3.13)
project(CommProtocol C)

set(CMAKE_C_STANDARD 99)

#add_executable(CommProtocol sock.c)
add_executable(CommProtocol serv.c 2client.c sock.c)