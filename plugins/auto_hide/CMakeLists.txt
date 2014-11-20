project (auto_hide)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	auto_hide.cpp
)

set (MOC_SOURCES
	auto_hide.h
)

set (CONFIGURATION_FILES
	configuration/auto_hide.ui
)

kadu_plugin (auto_hide
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES idle
)
