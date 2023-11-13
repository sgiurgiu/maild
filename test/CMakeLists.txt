cmake_minimum_required(VERSION 3.15)



set(CMAKE_INCLUDE_CURRENT_DIR ON)


find_package(Threads)
find_package(GTest CONFIG REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/lib ${CMAKE_CURRENT_SOURCE_DIR}/external)

set(LIBMAILD_TEST_SRCS 
    test_utils.cpp  
    )


add_executable(maild_test  ${LIBMAILD_TEST_SRCS})

target_link_libraries(maild_test PRIVATE maillib)
target_link_libraries(maild_test PRIVATE GTest::gtest GTest::gtest_main)
