project (simpleview)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	
	simpleview-config-ui.cpp
	simpleview-module.cpp
	simpleview-plugin-injector-factory.cpp
	simpleview-plugin-object.cpp
	simpleview.cpp
)

set (CONFIGURATION_FILES
	configuration/simpleview.ui
)

kadu_plugin (simpleview
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES docking
)
