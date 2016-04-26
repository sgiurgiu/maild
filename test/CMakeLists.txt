project(libmaild_test)

include_directories(${CMAKE_SOURCE_DIR}/lib ${CMAKE_CURRENT_SOURCE_DIR}/external)

set(LIBMAILD_TEST_SRCS 

  external/gmock-gtest-all.cc 
  external/gmock_main.cc)


add_executable(${PROJECT_NAME}  ${LIBMAILD_TEST_SRCS})

target_link_libraries(${PROJECT_NAME}  libmaild)
target_link_libraries(${PROJECT_NAME}  pthread)
