cmake_minimum_required(VERSION 3.15)
project(maild_web)

find_package(Boost 1.73 REQUIRED program_options filesystem)
find_package(OpenSSL REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0 gobject-2.0 gmodule-2.0 gio-2.0)
find_package(pcre2 CONFIG REQUIRED)
find_package(Crow CONFIG REQUIRED)
find_package(libpqxx CONFIG REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/lib ${Boost_INCLUDE_DIR})

set(MAILD_SRCS
        main.cpp
        options.cpp
        options_parser.cpp
        web_options.cpp
        web_server.cpp
        web_file_server.cpp
        web_not_found_exception.cpp
        web_api_server.cpp
    )

set(EXECUTABLE_NAME "maild_web")
add_executable(${EXECUTABLE_NAME}  ${MAILD_SRCS})

target_compile_definitions(${EXECUTABLE_NAME} PRIVATE MAILD_VERSION="${MAILD_VERSION}")
target_compile_definitions(${EXECUTABLE_NAME} PRIVATE MAILD_STRING="${MAILD_STRING}")

target_link_libraries(${EXECUTABLE_NAME} PRIVATE Threads::Threads)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE OpenSSL::SSL)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE libpqxx::pqxx)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE maillib)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE Boost::boost Boost::program_options Boost::filesystem)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE spdlog::spdlog_header_only fmt::fmt-header-only)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE PkgConfig::GLIB)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE PCRE2::8BIT PCRE2::16BIT PCRE2::32BIT PCRE2::POSIX)

install(TARGETS ${EXECUTABLE_NAME}  RUNTIME DESTINATION bin)

