cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)
project(log-plugin VERSION 0.4)

if (NOT LOG_PLUGIN_VERSION) # allow version override
	set(LOG_PLUGIN_VERSION ${PROJECT_VERSION})
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/libs/cmake")

# build static libraries
set(BUILD_SHARED_LIBS OFF)

if (MSVC)
	add_link_options(/INCREMENTAL:NO /NODEFAULTLIB:MSVCRT)
	# link runtime statically
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(UNIX)
	# hide non-exported symbols
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++98 -pedantic -fvisibility=hidden")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu89 -pedantic -fvisibility=hidden")
	set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS OFF)

	# link runtime statically
	# set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
	add_link_options(
		"-static-libgcc"
		"-static-libstdc++"
	)
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
