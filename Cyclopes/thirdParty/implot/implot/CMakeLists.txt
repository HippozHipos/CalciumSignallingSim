cmake_minimum_required(VERSION 3.8)
project("implot")

file(GLOB IMPLOT_HEADER_FILES 
    implot.h
)

file(GLOB IMPLOT_SOURCE_FILES 
    implot.cpp
    implot_internal.cpp
    implot_items.cpp

)

add_library(${PROJECT_NAME} ${IMPLOT_SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ../../imgui)
target_include_directories(${PROJECT_NAME} PUBLIC ../../imgui/backend/*.h)

target_link_libraries(${PROJECT_NAME} imgui)