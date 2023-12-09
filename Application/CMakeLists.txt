project(Application)
cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -fno-omit-frame-pointer -gdwarf-2")

file(GLOB SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/analyzer/analysis/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/analyzer/visuals/*.cpp
)

add_executable(${PROJECT_NAME} WIN32 ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${SIMULATOR_HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC ${CYCLOPES_HEADERS})
target_link_libraries(${PROJECT_NAME} Simulator Cyclopes)

