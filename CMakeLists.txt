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
pr_add_dependency(${PROJ_NAME} util_string TARGET PUBLIC)
pr_add_external_dependency(${PROJ_NAME} cpptrace LIBRARY PRIVATE)

pr_init_module(${PROJ_NAME})

# date
target_include_directories(${PROJ_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/third_party_libs/date/include/")
pr_add_module_list(${PROJ_NAME} cxx_modules_impl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/modules/date.cppm")

# magic_enum
target_include_directories(${PROJ_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/third_party_libs/magic_enum/include/")
pr_add_module_list(${PROJ_NAME} cxx_modules_impl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/third_party_libs/magic_enum/module/magic_enum.cppm")

# BS::thread-pool
target_include_directories(${PROJ_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/third_party_libs/thread-pool/include/")
pr_add_module_list(${PROJ_NAME} cxx_modules_impl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/third_party_libs/thread-pool/modules/BS.thread_pool.cppm")

# ctpl
target_include_directories(${PROJ_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/third_party_libs/CTPL/")
pr_add_module_list(${PROJ_NAME} cxx_modules_impl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/third_party_libs/CTPL/module/ctpl.cppm")

# spdlog
set(SPDLOG_BUILD_SHARED ON CACHE BOOL "Build shared library")
set(SPDLOG_USE_STD_FORMAT ON CACHE BOOL "Use std::format instead of fmt library.")
add_subdirectory(third_party_libs/spdlog)
pr_add_dependency(${PROJ_NAME} spdlog TARGET PUBLIC)
pr_add_module_list(${PROJ_NAME} cxx_modules_impl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/modules/spdlog.cppm")

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
