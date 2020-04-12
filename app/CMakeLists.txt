cmake_minimum_required(VERSION 3.15)
project(maild)

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)

find_package(Boost 1.72 REQUIRED program_options)
find_library(LOG4CXX_LIBS log4cxx)

include_directories(${CMAKE_SOURCE_DIR}/lib ${Boost_INCLUDE_DIR})

set(MAILD_SRCS main.cpp options.cpp options_parser.cpp)

set(EXECUTABLE_NAME "maild")
add_executable(${EXECUTABLE_NAME}  ${MAILD_SRCS})

target_link_libraries(${EXECUTABLE_NAME}  maillib)
target_link_libraries(${EXECUTABLE_NAME} ${Boost_LIBRARIES} ${LOG4CXX_LIBS})



install(TARGETS ${EXECUTABLE_NAME}  RUNTIME DESTINATION bin)
