project (single_window)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	single-window.cpp
	single-window-plugin.cpp
)

set (MOC_SOURCES
	single-window.h
	single-window-plugin.h
)

set (CONFIGURATION_FILES
	configuration/single_window.ui
)

kadu_plugin (single_window
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES docking
)
