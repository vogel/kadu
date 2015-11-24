project (docking_notify)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	docking-notify.cpp
	docking-notify-configuration-widget.cpp
	docking-notify-plugin.cpp
)

set (MOC_SOURCES
	docking-notify.h
	docking-notify-configuration-widget.h
	docking-notify-plugin.h
)

set (CONFIGURATION_FILES
	configuration/docking-notify.ui
)

kadu_plugin (docking_notify
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_DEPENDENCIES docking
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
