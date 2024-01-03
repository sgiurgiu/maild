cmake_minimum_required(VERSION 3.15)
project(libmaild)

find_library(GMIME_LIB gmime-3.0)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(pcre2 CONFIG REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0 gobject-2.0 gmodule-2.0 gio-2.0)
pkg_check_modules(IDN2 libidn2 IMPORTED_TARGET)

set(LIBMAILD_SRCS 
        utils.cpp
        )

set(LIBRARY_NAME "maillib")        
add_library(${LIBRARY_NAME} STATIC ${LIBMAILD_SRCS})

target_compile_definitions(${LIBRARY_NAME} PRIVATE SPDLOG_FMT_EXTERNAL_HO SPDLOG_FMT_EXTERNAL)
target_compile_definitions(${LIBRARY_NAME} PRIVATE "$<$<CONFIG:Debug>:MAILD_DEBUG>")
target_compile_options(${LIBRARY_NAME} PUBLIC $<$<AND:$<CONFIG:Debug>,$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>>:-Wall -pedantic -Wextra>)

target_link_libraries(${LIBRARY_NAME} PUBLIC PkgConfig::GLIB)
target_link_libraries(${LIBRARY_NAME} PUBLIC ${GMIME_LIB} )
target_link_libraries(${LIBRARY_NAME} PUBLIC PCRE2::8BIT PCRE2::16BIT PCRE2::32BIT PCRE2::POSIX)
target_link_libraries(${LIBRARY_NAME} PUBLIC PkgConfig::IDN2)
target_link_libraries(${LIBRARY_NAME} PRIVATE spdlog::spdlog_header_only fmt::fmt-header-only)


if (Tcmalloc_FOUND)
    target_link_libraries(${LIBRARY_NAME} PUBLIC ${Tcmalloc_LIBRARIES})
endif(Tcmalloc_FOUND)

install(TARGETS ${LIBRARY_NAME}  LIBRARY DESTINATION lib)
