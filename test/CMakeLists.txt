project(libmaild_test)
find_package(Threads)

include_directories(${CMAKE_SOURCE_DIR}/lib ${CMAKE_CURRENT_SOURCE_DIR}/external)

set(LIBMAILD_TEST_SRCS 
    test_utils.cpp
  external/gmock-gtest-all.cc 
  external/gmock_main.cc)


add_executable(${PROJECT_NAME}  ${LIBMAILD_TEST_SRCS})

target_link_libraries(${PROJECT_NAME}  maillib)
target_link_libraries(${PROJECT_NAME}  ${CMAKE_THREAD_LIBS_INIT})
