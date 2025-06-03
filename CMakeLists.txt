include(${CMAKE_SOURCE_DIR}/cmake/pr_common.cmake)

option(SHAREDUTILS_STATIC "Build as static library?" OFF)
option(LINK_COMMON_LIBS_STATIC "Link to common Pragma libraries statically?" OFF)

if(${SHAREDUTILS_STATIC})
	set(LIB_TYPE STATIC)
else()
	set(LIB_TYPE SHARED)
endif()

set(PROJ_NAME sharedutils)
pr_add_library(${PROJ_NAME} ${LIB_TYPE})

pr_add_dependency(${PROJ_NAME} mathutil TARGET PUBLIC)
pr_add_external_dependency(${PROJ_NAME} cpptrace LIBRARY PRIVATE)

pr_add_headers(${PROJ_NAME} "include/")
pr_add_sources(${PROJ_NAME} "src/")

if(${SHAREDUTILS_STATIC})
	pr_add_compile_definitions(${PROJ_NAME} -DSHUTIL_STATIC)
else()
	pr_add_compile_definitions(${PROJ_NAME} -DSHUTIL_DLL)
endif()

if(${LINK_COMMON_LIBS_STATIC})
	pr_add_compile_definitions(${PROJ_NAME} -DMUTIL_STATIC -DVFILESYSTEM_STATIC)
endif()

pr_add_compile_definitions(${PROJ_NAME} -DURI_STATIC_BUILD)

if(UNIX)
	target_link_libraries(${PROJ_NAME} PRIVATE "pthread")
	target_link_libraries(${PROJ_NAME} PRIVATE dl)
endif()

pr_finalize(${PROJ_NAME})
