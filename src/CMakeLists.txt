include(AMXConfig)
include(AddSAMPPlugin)


set(SAMP_SDK_ROOT "${PROJECT_SOURCE_DIR}/libs/sdk")
find_package(SAMPSDK REQUIRED)

set(LOG_PLUGIN_CONFIG_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/config_headers")

configure_file(
	"../log-plugin.inc.in"
	"log-plugin.inc"
	@ONLY
)

configure_file(
	"version.hpp.in"
	"${LOG_PLUGIN_CONFIG_INCLUDE_DIR}/version.hpp"
	@ONLY
)


add_samp_plugin(log-plugin 
	${SAMPSDK_DIR}/amxplugin.cpp
	${SAMPSDK_DIR}/amxplugin2.cpp
	PluginLog.cpp
	PluginLog.hpp
	LogManager.cpp
	LogManager.hpp
	natives.cpp
	natives.hpp
	SampConfigReader.cpp
	SampConfigReader.hpp
	ServerLogHook.cpp
	ServerLogHook.hpp
	Singleton.hpp
	main.cpp
	sdk.hpp
	plugin.def
)

if (UNIX)
	target_sources(log-plugin PUBLIC ${SAMPSDK_DIR}/amx/getch.c)
endif()

set_property(TARGET log-plugin PROPERTY CXX_STANDARD 11)
set_property(TARGET log-plugin PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(log-plugin PRIVATE
	${SAMPSDK_INCLUDE_DIR}
	${LOG_PLUGIN_CONFIG_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/libs/log-core/include
)

if(MSVC)
	target_compile_definitions(log-plugin PRIVATE
		BOOST_ALL_NO_LIB # disable auto-linking
		_CRT_SECURE_NO_WARNINGS
		NOMINMAX
		_WIN32_WINNT=0x0600
		_SCL_SECURE_NO_WARNINGS
	)
endif()

target_link_libraries(log-plugin log-core fmt subhook)

if(UNIX)
	set_target_properties(log-plugin PROPERTIES
		INSTALL_RPATH "$ORIGIN/..:$ORIGIN/")
endif()

# increase warning levels
if (MSVC)
	target_compile_options(log-plugin PRIVATE
		/W4
		/WX
		/permissive-
	)
elseif(UNIX)
	target_compile_options(log-plugin PRIVATE
		-Wall
		-Wextra
		-pedantic
	)
endif()


install(TARGETS log-plugin DESTINATION "plugins/")
get_target_property(LOGCORE_LOCATION log-core LOCATION)
install(FILES "${LOGCORE_LOCATION}" DESTINATION "./")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/log-plugin.inc" DESTINATION "pawno/include/")
install(FILES "${PROJECT_SOURCE_DIR}/LICENSE" DESTINATION "./")


set(CPACK_PACKAGE_VERSION ${LOG_PLUGIN_VERSION})
if(WIN32)
	set(CPACK_GENERATOR ZIP)
else()
	set(CPACK_GENERATOR TGZ)
endif()

include(CPack)
