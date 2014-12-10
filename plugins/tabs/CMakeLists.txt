project (tabs)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	tabs.cpp
	tabs-chat-widget-container-handler.cpp
	tabs-plugin.cpp
	gui/widgets/tab-bar.cpp
	gui/widgets/tab-widget.cpp
)

set (MOC_SOURCES
	tabs.h
	tabs-plugin.h
	gui/widgets/tab-bar.h
	gui/widgets/tab-widget.h
)

set (CONFIGURATION_FILES
	configuration/tabs.ui
)

if (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	include_directories (${X11_X11_INCLUDE_PATH})
endif ()

kadu_plugin (tabs
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
