cmake_minimum_required(VERSION 3.2)
project(maild)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall -pedantic -Wextra")

SET(MAILD_VERSION_MAJOR "0")
SET(MAILD_VERSION_MINOR "0")
SET(MAILD_VERSION_PATCH "1")

add_definitions(-DMAILD_VERSION="${MAILD_VERSION_MAJOR}.${MAILD_VERSION_MINOR}.${MAILD_VERSION_PATCH}")

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()


add_subdirectory(lib)
add_subdirectory(app)
add_subdirectory(test)
add_subdirectory(web)
