project (single_window)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	single-window.cpp
	single-window-chat-widget-container-handler.cpp
	single-window-plugin.cpp
)

set (MOC_SOURCES
	single-window.h
	single-window-plugin.h
)

set (CONFIGURATION_FILES
	configuration/single_window.ui
)

kadu_plugin (single_window
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
