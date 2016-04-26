cmake_minimum_required(VERSION 3.2)
project(libmaild)

find_package( Boost 1.53 REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(${Boost_INCLUDE_DIR})

if(CMAKE_BUILD_TYPE MATCHES Debug)  
    add_definitions(-DMAILD_DEBUG)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

set(LIBMAILD_SRCS server.cpp)

add_library(${PROJECT_NAME}  STATIC ${LIBMAILD_SRCS})


target_link_libraries(${PROJECT_NAME}  pthread)
target_link_libraries(${PROJECT_NAME} ${OPENSSL_CRYPTO_LIBRARY})
