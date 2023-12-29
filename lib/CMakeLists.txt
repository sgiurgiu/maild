cmake_minimum_required(VERSION 3.15)
project(libmaild)

find_package(Boost 1.73 REQUIRED filesystem system)
find_package(OpenSSL REQUIRED)
find_package(libpqxx CONFIG REQUIRED)
find_package(Threads)
find_library(MAGIC_LIB magic)
find_library(GMIME_LIB gmime-3.0)
#find_library(MIMETIC_LIB mimetic)
#this is needed when pqxx is statically compiled
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(Crow CONFIG REQUIRED)
find_package(pcre2 CONFIG REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0 gobject-2.0 gmodule-2.0 gio-2.0)
pkg_check_modules(IDN2 libidn2 IMPORTED_TARGET)

set(LIBMAILD_SRCS 
        smtp_server.cpp 
        server_options.cpp 
        server_manager.cpp 
        session.cpp 
        web_options.cpp
        web_server.cpp
        web_file_server.cpp
        web_not_found_exception.cpp
        magic_handler.cpp
        web_api_server.cpp
        utils.cpp
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
        starttls_command.cpp)

set(LIBRARY_NAME "maillib")        
add_library(${LIBRARY_NAME} STATIC ${LIBMAILD_SRCS})

target_compile_definitions(${LIBRARY_NAME} PRIVATE SPDLOG_FMT_EXTERNAL_HO SPDLOG_FMT_EXTERNAL)
target_compile_definitions(${LIBRARY_NAME} PRIVATE BOOST_ASIO_USE_TS_EXECUTOR_AS_DEFAULT)
target_compile_definitions(${LIBRARY_NAME} PRIVATE "$<$<CONFIG:Debug>:MAILD_DEBUG>")
target_compile_definitions(${LIBRARY_NAME} PRIVATE MAILD_VERSION="${MAILD_VERSION}")
target_compile_definitions(${LIBRARY_NAME} PRIVATE MAILD_STRING="${MAILD_STRING}")

target_compile_options(${LIBRARY_NAME} PUBLIC $<$<AND:$<CONFIG:Debug>,$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>>:-Wall -pedantic -Wextra>)

target_link_libraries(${LIBRARY_NAME} PUBLIC Threads::Threads)
target_link_libraries(${LIBRARY_NAME} PUBLIC OpenSSL::SSL)
target_link_libraries(${LIBRARY_NAME} PUBLIC Boost::boost Boost::filesystem)
target_link_libraries(${LIBRARY_NAME} PUBLIC libpqxx::pqxx)
target_link_libraries(${LIBRARY_NAME} PUBLIC ${MAGIC_LIB})
target_link_libraries(${LIBRARY_NAME} PUBLIC PkgConfig::GLIB)
target_link_libraries(${LIBRARY_NAME} PUBLIC ${GMIME_LIB} )
target_link_libraries(${LIBRARY_NAME} PUBLIC PCRE2::8BIT PCRE2::16BIT PCRE2::32BIT PCRE2::POSIX)
target_link_libraries(${LIBRARY_NAME} PUBLIC PkgConfig::IDN2)

#target_link_libraries(${LIBRARY_NAME} PUBLIC ${MIMETIC_LIB})
target_link_libraries(${LIBRARY_NAME} PRIVATE Crow::Crow)
target_link_libraries(${LIBRARY_NAME} PRIVATE spdlog::spdlog_header_only fmt::fmt-header-only)


if (Tcmalloc_FOUND)
    target_link_libraries(${LIBRARY_NAME} PUBLIC ${Tcmalloc_LIBRARIES})
endif(Tcmalloc_FOUND)

install(TARGETS ${LIBRARY_NAME}  LIBRARY DESTINATION lib)
