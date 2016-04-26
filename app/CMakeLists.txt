cmake_minimum_required(VERSION 3.2)
project(maild)

include_directories(${CMAKE_SOURCE_DIR}/lib)

set(MAILD_SRCS main.cpp)

add_executable(${PROJECT_NAME}  ${MAILD_SRCS})

target_link_libraries(${PROJECT_NAME}  libmaild)
#target_link_libraries(${PROJECT_NAME} ${OPENSSL_CRYPTO_LIBRARY})

install(TARGETS ${PROJECT_NAME}  RUNTIME DESTINATION bin)
