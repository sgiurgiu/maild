cmake_minimum_required(VERSION 3.2)
project(maild)

set(Boost_USE_STATIC_LIBS        OFF)
set(Boost_USE_MULTITHREADED      ON)

find_package(Boost 1.58 REQUIRED filesystem program_options)
find_library(LOG4CXX_LIBS log4cxx)

include_directories(${CMAKE_SOURCE_DIR}/lib ${Boost_INCLUDE_DIR})

set(MAILD_SRCS main.cpp options.cpp options_parser.cpp)

add_executable(${PROJECT_NAME}  ${MAILD_SRCS})

target_link_libraries(${PROJECT_NAME}  libmaild)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${LOG4CXX_LIBS})



install(TARGETS ${PROJECT_NAME}  RUNTIME DESTINATION bin)