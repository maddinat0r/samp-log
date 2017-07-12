cmake_minimum_required(VERSION 3.1)
project(log-plugin)

set(LOG_PLUGIN_VERSION "0.4")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/libs/cmake")

if(UNIX)
	#force 32bit compilation and hide non-exported symbols
	set(CMAKE_CXX_FLAGS "-m32 -fvisibility=hidden")
	set(CMAKE_C_FLAGS "-m32 -fvisibility=hidden")
	set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS OFF)
endif()

option(LOGCORE_INSTALL_DEV OFF)
mark_as_advanced(LOGCORE_INSTALL_DEV)
add_subdirectory(libs/log-core)

add_subdirectory(src)
