project (indicator_docking)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)
find_package (PkgConfig)
pkg_search_module (LIBMESSAGINGMENU REQUIRED messaging-menu)

set (SOURCES
	libqmessagingmenu/qmessaging-menu-app.cpp
	libqmessagingmenu/qmessaging-menu-source.cpp

	indicator-docking.cpp
	indicator-docking-plugin.cpp
)

link_directories (${LIBMESSAGINGMENU_LIBRARY_DIRS})
add_definitions (${LIBMESSAGINGMENU_CFLAGS_OTHER})
include_directories (${LIBMESSAGINGMENU_INCLUDE_DIRS})

kadu_plugin (indicator_docking
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_LIBRARIES ${LIBMESSAGINGMENU_LIBRARIES}
)
