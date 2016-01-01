project (simpleview)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	simpleview.cpp
	simpleview-config-ui.cpp
	simpleview-plugin.cpp
)

set (CONFIGURATION_FILES
	configuration/simpleview.ui
)

kadu_plugin (simpleview
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES docking
)
