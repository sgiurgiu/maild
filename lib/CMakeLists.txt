cmake_minimum_required(VERSION 3.2)
project(libmaild)

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)

find_package(Boost 1.70 REQUIRED)
find_package(OpenSSL REQUIRED)
find_library(LOG4CXX_LIBS log4cxx)
find_library(PSQLCXX_LIBS pqxx)
find_package(Tcmalloc)
find_package(Threads)
find_library(MAGIC_LIB magic)
find_library(MIMETIC_LIB mimetic)

include_directories(${Boost_INCLUDE_DIR} ${BEASTHTTP_INCLUDE})

if(CMAKE_BUILD_TYPE MATCHES Debug)  
    add_definitions(-DMAILD_DEBUG)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

set(LIBMAILD_SRCS 
        smtp_server.cpp 
        server_options.cpp 
        server_manager.cpp 
        session.cpp 
        active_object.cpp
        web_options.cpp
        web_server.cpp
        web_file_server.cpp
        web_not_found_exception.cpp
        magic_handler.cpp
        web_api_server.cpp
        utils.cpp)

set(LIBRARY_NAME "maillib")        
add_library(${LIBRARY_NAME} STATIC ${LIBMAILD_SRCS})


target_link_libraries(${LIBRARY_NAME} PUBLIC  ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${LIBRARY_NAME} PUBLIC ${OPENSSL_CRYPTO_LIBRARY})
target_link_libraries(${LIBRARY_NAME} PUBLIC ${Boost_LIBRARIES} ${LOG4CXX_LIBS})
target_link_libraries(${LIBRARY_NAME} PUBLIC ${PSQLCXX_LIBS})
target_link_libraries(${LIBRARY_NAME} PUBLIC ${MAGIC_LIB} ${MIMETIC_LIB})

if (Tcmalloc_FOUND)
    target_link_libraries(${LIBRARY_NAME} ${Tcmalloc_LIBRARIES})
endif(Tcmalloc_FOUND)

install(TARGETS ${LIBRARY_NAME}  LIBRARY DESTINATION lib)
