project (hints)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	hint.cpp
	hints-configuration.cpp
	hints-module.cpp
	hints-notifier.cpp
	hints-plugin-modules-factory.cpp
	hints-plugin-object.cpp
	hints-widget-positioner.cpp
	hints-widget.cpp
)

set (CONFIGURATION_FILES
	configuration/hints.ui
)

kadu_plugin (hints
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
