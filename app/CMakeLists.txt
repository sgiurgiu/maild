cmake_minimum_required(VERSION 3.15)
project(maild)

find_package(Boost 1.73 REQUIRED program_options)

include_directories(${CMAKE_SOURCE_DIR}/lib ${Boost_INCLUDE_DIR})

set(MAILD_SRCS main.cpp options.cpp options_parser.cpp)

set(EXECUTABLE_NAME "maild")
add_executable(${EXECUTABLE_NAME}  ${MAILD_SRCS})

target_link_libraries(${EXECUTABLE_NAME} PUBLIC maillib)
target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${Boost_LIBRARIES})
target_link_libraries(${EXECUTABLE_NAME} PRIVATE spdlog::spdlog_header_only fmt::fmt-header-only)


install(TARGETS ${EXECUTABLE_NAME}  RUNTIME DESTINATION bin)
