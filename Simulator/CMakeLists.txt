project(Simulator)
cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 20)

file(GLOB SRC_FILES
   ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(${PROJECT_NAME} STATIC ${SRC_FILES})

set(SIMULATOR_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    CACHE INTERNAL ""
)

