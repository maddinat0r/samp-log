cmake_minimum_required(VERSION 3.13)
project(log-plugin VERSION 0.4)

if (NOT LOG_PLUGIN_VERSION) # allow version override
	set(LOG_PLUGIN_VERSION ${PROJECT_VERSION})
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/libs/cmake")

if(UNIX)
	# force 32bit compilation and hide non-exported symbols
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -fvisibility=hidden")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 -fvisibility=hidden")
	set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS OFF)
endif()


add_subdirectory(libs/fmt)
mark_as_advanced(FMT_TEST FMT_INSTALL FMT_PEDANTIC FMT_DOC FMT_USE_CPP11 FMT_WERROR)


find_package(log-core REQUIRED CONFIG)


set(SUBHOOK_STATIC ON CACHE BOOL "")
set(SUBHOOK_INSTALL OFF CACHE BOOL "")
set(SUBHOOK_TESTS OFF CACHE BOOL "")
set(SUBHOOK_FORCE_32BIT ON CACHE BOOL "")
set(BUILD_TESTING OFF CACHE BOOL "")
mark_as_advanced(SUBHOOK_STATIC SUBHOOK_INSTALL SUBHOOK_TESTS SUBHOOK_FORCE_32BIT BUILD_TESTING)
add_subdirectory(libs/subhook)


add_subdirectory(src)
