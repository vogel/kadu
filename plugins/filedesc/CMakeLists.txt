project (filedesc)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	filedesc-module.cpp
	filedesc-plugin-injector-factory.cpp
	filedesc-plugin-object.cpp
	filedesc-status-changer.cpp
	filedesc.cpp
)

set (CONFIGURATION_FILES
	configuration/filedesc.ui
)

kadu_plugin (filedesc
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
