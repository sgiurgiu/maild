cmake_minimum_required(VERSION 3.2)
project(libmaild)

set(Boost_USE_STATIC_LIBS        OFF)
set(Boost_USE_MULTITHREADED      ON)

find_package(Boost 1.58 REQUIRED system filesystem)
find_package(OpenSSL REQUIRED)
find_library(LOG4CXX_LIBS log4cxx)
find_library(PSQLCXX_LIBS pqxx)
find_package(Tcmalloc)
find_package(Threads)

include_directories(${Boost_INCLUDE_DIR})

if(CMAKE_BUILD_TYPE MATCHES Debug)  
    add_definitions(-DMAILD_DEBUG)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

set(LIBMAILD_SRCS 
        smtp_server.cpp 
        server_options.cpp 
        server_manager.cpp 
        session.cpp 
        active_object.cpp)

add_library(${PROJECT_NAME}  SHARED ${LIBMAILD_SRCS})


target_link_libraries(${PROJECT_NAME}  ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${PROJECT_NAME} ${OPENSSL_CRYPTO_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${LOG4CXX_LIBS})
target_link_libraries(${PROJECT_NAME}  ${PSQLCXX_LIBS})

if (Tcmalloc_FOUND)
    target_link_libraries(${PROJECT_NAME} ${Tcmalloc_LIBRARIES})
endif(Tcmalloc_FOUND)