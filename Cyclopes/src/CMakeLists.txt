cmake_minimum_required (VERSION 3.8)

set (CMAKE_CXX_STANDARD 17)

project("Cyclopes")

set(OUTPUT_PATH ${CMAKE_SOURCE_DIR}/out/build CACHE INTERNAL "")
set(THIRD_PARTY ${CMAKE_SOURCE_DIR}/Cyclopes/thirdParty)
set(CYCLOPES_HEADERS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")

file(GLOB_RECURSE CYC_SOURCE 
	${CMAKE_CURRENT_SOURCE_DIR}/**.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Cyclopes/core/**.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Cyclopes/Event/**.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Cyclopes/imgui/**.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Cyclopes/Platform/**.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Cyclopes/Renderer/**.cpp
	${CMAKE_SOURCE_DIR}/Cyclopes/thirdParty/stbimg/*.cpp
)

add_library (${PROJECT_NAME}  ${CYC_SOURCE})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Cyclopes/src)
target_include_directories(${PROJECT_NAME} PUBLIC  ${THIRD_PARTY})

#assimp
#set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
#set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Assimp build tests" FORCE)
#set(ASSIMP_INSTALL OFF CACHE BOOL "Assimp install" FORCE)
#set(ASSIMP_INSTALL_PDB OFF CACHE BOOL "Assimp install PDB" FORCE)
#target_include_directories(${PROJECT_NAME} PUBLIC ${THIRD_PARTY}/assimp/include)
#target_include_directories(${PROJECT_NAME} PUBLIC ${OUTPUT_PATH}/assimp/include)
#add_subdirectory(${THIRD_PARTY}/assimp ${OUTPUT_PATH}/assimp)

#glm
target_include_directories(${PROJECT_NAME} PUBLIC ${THIRD_PARTY}/glm)

#glad
target_include_directories(${PROJECT_NAME} PUBLIC ${THIRD_PARTY}/glad/include)
add_subdirectory(${THIRD_PARTY}/glad ${OUTPUT_PATH}/glad)

#imgui
target_include_directories(${PROJECT_NAME} PUBLIC ${THIRD_PARTY}/imgui)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backend/*.h)
add_subdirectory(${THIRD_PARTY}/imgui ${OUTPUT_PATH}/imgui)

#implot
target_include_directories(${PROJECT_NAME} PUBLIC ${THIRD_PARTY}/implot/implot)
add_subdirectory(${THIRD_PARTY}/implot/implot ${OUTPUT_PATH}/implot)

#spdlog
target_include_directories(${PROJECT_NAME} PUBLIC ${THIRD_PARTY}/spdlog/include)
add_subdirectory(${THIRD_PARTY}/spdlog ${OUTPUT_PATH}/spdlog)

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})


target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} glad imgui implot)


