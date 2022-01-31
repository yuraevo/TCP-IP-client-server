cmake_minimum_required(VERSION 3.2) 

project(Server)

add_executable(server src/server.cpp)
add_executable(client src/client.cpp)
add_executable(business_logic src/business_logic.cpp)
