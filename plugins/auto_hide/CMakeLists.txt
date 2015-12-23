project (auto_hide)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	auto-hide-configuration-ui-handler.cpp
	auto-hide-module.cpp
	auto-hide-plugin-injector-factory.cpp
	auto-hide-plugin-object.cpp
	auto-hide.cpp
)

set (CONFIGURATION_FILES
	configuration/auto_hide.ui
)

kadu_plugin (auto_hide
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES idle
)
