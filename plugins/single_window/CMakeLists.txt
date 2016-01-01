project (single_window)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	single-window.cpp
	single-window-chat-widget-container-handler.cpp
	single-window-plugin.cpp
)

set (CONFIGURATION_FILES
	configuration/single_window.ui
)

if (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	include_directories (${X11_X11_INCLUDE_PATH})
endif ()

kadu_plugin (single_window
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
