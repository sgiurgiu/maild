cmake_minimum_required(VERSION 3.15)

find_package(Boost 1.73 REQUIRED program_options)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/lib ${Boost_INCLUDE_DIR})

set(MAILD_SRCS main.cpp options.cpp options_parser.cpp)

set(EXECUTABLE_NAME "maild")
add_executable(${EXECUTABLE_NAME}  ${MAILD_SRCS})

target_compile_definitions(${EXECUTABLE_NAME} PRIVATE MAILD_VERSION="${MAILD_VERSION}")

target_link_libraries(${EXECUTABLE_NAME} PUBLIC maillib)
target_link_libraries(${EXECUTABLE_NAME} PUBLIC Boost::program_options)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE spdlog::spdlog_header_only fmt::fmt-header-only)


install(TARGETS ${EXECUTABLE_NAME}  RUNTIME DESTINATION bin)
