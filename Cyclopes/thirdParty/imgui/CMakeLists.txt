cmake_minimum_required(VERSION 3.8)
project("imgui")

file(GLOB IMGUI_HEADER_FILES 
    imgui.h
    imconfig.h
    imgui_internal.h
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
  
    backends/imgui_impl_win32.h
    backends/imgui_impl_opengl3.h
)

file(GLOB IMGUI_SOURCE_FILES 
     imgui.cpp  
     imgui_demo.cpp
     imgui_draw.cpp
     imgui_tables.cpp
     imgui_widgets.cpp
)

add_library(${PROJECT_NAME} ${IMGUI_SOURCE_FILES})
