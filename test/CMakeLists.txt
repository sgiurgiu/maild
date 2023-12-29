cmake_minimum_required(VERSION 3.15)



set(CMAKE_INCLUDE_CURRENT_DIR ON)


find_package(Threads)
find_package(GTest CONFIG REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0 gobject-2.0 gmodule-2.0 gio-2.0)
find_package(pcre2 CONFIG REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/lib ${CMAKE_CURRENT_SOURCE_DIR}/external)

set(LIBMAILD_TEST_SRCS 
    test_utils.cpp  
    )


add_executable(maild_test  ${LIBMAILD_TEST_SRCS})

target_link_libraries(maild_test PRIVATE maillib)
target_link_libraries(maild_test PRIVATE GTest::gtest GTest::gtest_main)
target_link_libraries(maild_test PUBLIC PkgConfig::GLIB)
target_link_libraries(maild_test PUBLIC PCRE2::8BIT PCRE2::16BIT PCRE2::32BIT PCRE2::POSIX)
