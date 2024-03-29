cmake_minimum_required(VERSION 3.12)

set(PROJ_NAME sharedutils)
project(${PROJ_NAME} C CXX)

if(UNIX)
	include(ExternalProject)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 20)

set(DEPENDENCY_GLM_INCLUDE "" CACHE PATH "Path to GLM include directory.")
set(DEPENDENCY_MATHUTIL_INCLUDE "" CACHE PATH "Path to sharedutils include directory.")
set(DEPENDENCY_MATHUTIL_LIBRARY "" CACHE FILEPATH "Path to mathutil library. Required if this is compiled as a shared library.")
option(SHAREDUTILS_STATIC "Build as static library?" OFF)
option(LINK_COMMON_LIBS_STATIC "Link to common Pragma libraries statically?" OFF)

if(${SHAREDUTILS_STATIC})
	set(LIB_TYPE STATIC)
	add_definitions(-DSHUTIL_STATIC)
else()
	set(LIB_TYPE SHARED)
	add_definitions(-DSHUTIL_DLL)
endif()

if(${LINK_COMMON_LIBS_STATIC})
	add_definitions(-DMUTIL_STATIC)
	add_definitions(-DVFILESYSTEM_STATIC)
endif()

add_definitions(-DGLM_FORCE_DEPTH_ZERO_TO_ONE)
add_definitions(-DURI_STATIC_BUILD)

function(def_vs_filters FILE_LIST)
	foreach(source IN LISTS FILE_LIST)
	    get_filename_component(source_path "${source}" PATH)
	    string(REPLACE "${CMAKE_CURRENT_LIST_DIR}" "" source_path_relative "${source_path}")
	    string(REPLACE "/" "\\" source_path_msvc "${source_path_relative}")
	    source_group("${source_path_msvc}" FILES "${source}")
	endforeach()
endfunction(def_vs_filters)

file(GLOB_RECURSE SRC_FILES
    "${CMAKE_CURRENT_LIST_DIR}/src/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/include/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/include/*.hpp"
)
def_vs_filters("${SRC_FILES}")
add_library(${PROJ_NAME} ${LIB_TYPE} ${SRC_FILES})
if(WIN32)
	target_compile_options(${PROJ_NAME} PRIVATE /wd4251)
	target_compile_options(${PROJ_NAME} PRIVATE /wd4996)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${PROJ_NAME} PRIVATE -Wno-c++11-narrowing)
endif()
target_link_libraries(${PROJ_NAME} ${DEPENDENCY_MATHUTIL_LIBRARY})

set_target_properties(${PROJ_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${PROJ_NAME} PRIVATE ${DEPENDENCY_GLM_INCLUDE})
target_include_directories(${PROJ_NAME} PRIVATE ${DEPENDENCY_MATHUTIL_INCLUDE})

if(UNIX)
	target_link_libraries(${PROJ_NAME} "pthread")
	target_link_libraries(${PROJ_NAME} dl)
	target_link_libraries(${PROJ_NAME} stdc++fs)
endif()
