cmake_minimum_required(VERSION 3.2) 

project(Server)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.76.0 COMPONENTS filesystem regex) 

if(Boost_FOUND)
    add_executable(server src/server.cpp)
    add_executable(client src/client.cpp)
    add_executable(business_logic src/business_logic.cpp)
    target_link_libraries(business_logic Boost::boost -lpthread -lm)
    target_link_libraries(server Boost::boost -lpthread -lm)
endif()
