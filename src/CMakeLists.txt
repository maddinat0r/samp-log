include(AMXConfig)
include(AddSAMPPlugin)


set(SAMP_SDK_ROOT "${PROJECT_SOURCE_DIR}/libs/sdk")
find_package(SAMPSDK REQUIRED)

set(LOG_PLUGIN_CONFIG_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/config_headers")
include_directories(
	${SAMPSDK_INCLUDE_DIR}
	${LOG_PLUGIN_CONFIG_INCLUDE_DIR}
	${PROJECT_SOURCE_DIR}/libs/log-core/include
)

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
	${SAMPSDK_DIR}/amx/getch.c
	PluginLog.cpp
	PluginLog.hpp
	LogManager.cpp
	LogManager.hpp
	natives.cpp
	natives.hpp
	CSingleton.hpp
	main.cpp
	sdk.hpp
	plugin.def
)

target_compile_features(log-plugin PUBLIC 
	cxx_auto_type 
	cxx_decltype 
	cxx_defaulted_functions 
	cxx_default_function_template_args
	cxx_delegating_constructors
	cxx_deleted_functions
	cxx_enum_forward_declarations
	cxx_explicit_conversions
	cxx_extended_friend_declarations
	cxx_lambdas
	cxx_inheriting_constructors
	cxx_noexcept
	cxx_nonstatic_member_init
	cxx_nullptr
	cxx_range_for
	cxx_reference_qualified_functions
	cxx_right_angle_brackets
	cxx_rvalue_references
	cxx_strong_enums
	cxx_variadic_templates
)

add_dependencies(log-plugin log-core fmt)

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -DNOMINMAX)
endif()

target_link_libraries(log-plugin log-core fmt)

if(UNIX)
	set_target_properties(log-plugin PROPERTIES
		INSTALL_RPATH "$ORIGIN/..:$ORIGIN/")
endif()

install(TARGETS log-plugin DESTINATION "plugins/")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/log-plugin.inc" DESTINATION "pawno/include/")

set(CPACK_PACKAGE_VERSION ${LOG_PLUGIN_VERSION})
if(WIN32)
	set(CPACK_GENERATOR ZIP)
else()
	set(CPACK_GENERATOR TGZ)
endif()

include(CPack)
