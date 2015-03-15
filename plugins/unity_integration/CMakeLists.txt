project (unity_integration)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)
find_package (PkgConfig)

message (STATUS "If unity is not found and you are not compiling for one of Ubuntu flawors, disable unity_integration in Plugins.cmake")
pkg_search_module (LIBUNITY REQUIRED unity)

set (SOURCES
	libqunity/qunity.cpp

	unity-integration.cpp
	unity-integration-plugin.cpp
)

link_directories (${LIBUNITY_LIBRARY_DIRS})
add_definitions (${LIBUNITY_CFLAGS_OTHER})
include_directories (${LIBUNITY_INCLUDE_DIRS})

kadu_plugin (unity_integration
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_LIBRARIES ${LIBUNITY_LIBRARIES}
)
