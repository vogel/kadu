project (desktop_docking)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	configuration/gui/desktop-dock-configuration-ui-handler.cpp
	gui/windows/desktop-dock-window.cpp
	desktop-docking-plugin.cpp
	desktop-dock.cpp
)

set (MOC_SOURCES
	configuration/gui/desktop-dock-configuration-ui-handler.h
	gui/windows/desktop-dock-window.h
	desktop-docking-plugin.h
	desktop-dock.h
)

set (CONFIGURATION_FILES
	data/configuration/desktop_docking.ui
)

kadu_plugin (desktop_docking
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES docking
)
