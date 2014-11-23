project (indicator_docking)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	indicator_docking.cpp
	indicator_docking-plugin.cpp
)

find_package (PkgConfig)
pkg_search_module (LIBMESSAGINGMENU REQUIRED messaging-menu)

link_directories (${LIBMESSAGINGMENU_LIBRARY_DIRS})
add_definitions (${LIBMESSAGINGMENU_CFLAGS_OTHER})
include_directories (${LIBMESSAGINGMENU_INCLUDE_DIRS})

kadu_plugin (indicator_docking
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_LIBRARIES ${LIBINDICATE_LIBRARIES}
	PLUGIN_DEPENDENCIES docking
)
