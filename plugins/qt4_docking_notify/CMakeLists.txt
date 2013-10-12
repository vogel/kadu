project (qt4_docking_notify)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	qt4-docking-notify.cpp
	qt4-docking-notify-configuration-widget.cpp
	qt4-docking-notify-plugin.cpp
)

set (MOC_SOURCES
	qt4-docking-notify.h
	qt4-docking-notify-configuration-widget.h
	qt4-docking-notify-plugin.h
)

set (CONFIGURATION_FILES
	configuration/qt4-docking-notify.ui
)

kadu_plugin (qt4_docking_notify
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_DEPENDENCIES qt4_docking
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
