project (mediaplayer)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

kadu_api_directories (plugins/mediaplayer
	.
)

set (SOURCES
	notify/mediaplayer-notification.cpp

	mediaplayer.cpp
	mediaplayer-plugin.cpp
	media-player-status-changer.cpp
)

set (MOC_SOURCES
	notify/mediaplayer-notification.h

	mediaplayer.h
	mediaplayer-plugin.h
	player_commands.h
)

set (CONFIGURATION_FILES
	configuration/mediaplayer.ui
)

set (DATA_FILES
	data/mediaplayer.png
)

kadu_plugin (mediaplayer
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES docking
	PLUGIN_LIBRARIES ${DBUS_LIBS}
	PLUGIN_DATA_FILES ${DATA_FILES}
)
