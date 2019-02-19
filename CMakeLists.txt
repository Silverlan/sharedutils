cmake_minimum_required(VERSION 3.12)

set(PROJ_NAME sharedutils)
project(${PROJ_NAME} CXX)

set(CMAKE_CXX_STANDARD 17)

set(DEPENDENCY_GLM_INCLUDE "" CACHE PATH "Path to GLM include directory.")
set(DEPENDENCY_MATHUTIL_INCLUDE "" CACHE PATH "Path to sharedutils include directory.")
set(DEPENDENCY_MATHUTIL_LIBRARY "" CACHE FILEPATH "Path to mathutil library. Required if this is compiled as a shared library.")
set(SHAREDUTILS_STATIC 0 CACHE BOOL "Build as static library?")

if(${SHAREDUTILS_STATIC} MATCHES ON)
	set(LIB_TYPE STATIC)
else()
	set(LIB_TYPE SHARED)
endif()

add_definitions(-DSHUTIL_DLL)
add_definitions(-DGLM_FORCE_DEPTH_ZERO_TO_ONE)

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
    "${CMAKE_CURRENT_LIST_DIR}/include/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/include/*.hpp"
)
def_vs_filters("${SRC_FILES}")
add_library(${PROJ_NAME} ${LIB_TYPE} ${SRC_FILES})

target_link_libraries(${PROJ_NAME} ${DEPENDENCY_MATHUTIL_LIBRARY})

set_target_properties(${PROJ_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJ_NAME} PRIVATE ${DEPENDENCY_GLM_INCLUDE})
target_include_directories(${PROJ_NAME} PRIVATE ${DEPENDENCY_MATHUTIL_INCLUDE})
