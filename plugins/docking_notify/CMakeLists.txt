project (docking_notify)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	docking-notifier.cpp
	docking-notify-configuration-widget.cpp
	docking-notify-module.cpp
	docking-notify-plugin-injector-factory.cpp
	docking-notify-plugin-object.cpp
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
