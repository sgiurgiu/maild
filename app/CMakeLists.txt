cmake_minimum_required(VERSION 3.15)

find_package(Boost 1.73 REQUIRED program_options system)
find_package(OpenSSL REQUIRED)
find_package(libpqxx CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0 gobject-2.0 gmodule-2.0 gio-2.0)
find_package(pcre2 CONFIG REQUIRED)


include_directories(${CMAKE_SOURCE_DIR}/lib ${Boost_INCLUDE_DIR})

set(MAILD_SRCS 
    main.cpp
    options.cpp
    options_parser.cpp
    smtp_server.cpp 
    server_options.cpp 
    server_manager.cpp 
    session.cpp 
    smtp_command.cpp
    hello_command.cpp
    ehlo_command.cpp
    mail_command.cpp
    rcpt_command.cpp
    data_command.cpp
    quit_command.cpp
    auth_command.cpp
    rset_command.cpp
    help_command.cpp
    verify_command.cpp
    starttls_command.cpp
    )

set(EXECUTABLE_NAME "maild")
add_executable(${EXECUTABLE_NAME}  ${MAILD_SRCS})

target_compile_definitions(${EXECUTABLE_NAME} PRIVATE MAILD_VERSION="${MAILD_VERSION}")

target_link_libraries(${EXECUTABLE_NAME} PRIVATE Threads::Threads)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE OpenSSL::SSL)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE libpqxx::pqxx)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE maillib)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE Boost::boost Boost::program_options)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE spdlog::spdlog_header_only fmt::fmt-header-only)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE PkgConfig::GLIB)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE PCRE2::8BIT PCRE2::16BIT PCRE2::32BIT PCRE2::POSIX)

install(TARGETS ${EXECUTABLE_NAME}  RUNTIME DESTINATION bin)
