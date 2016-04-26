cmake_minimum_required(VERSION 2.6)
project(maild)

add_executable(maild main.cpp)

install(TARGETS maild RUNTIME DESTINATION bin)
