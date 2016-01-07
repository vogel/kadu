project (mediaplayer)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

kadu_api_directories (plugins/mediaplayer
	.
)

set (SOURCES
	notification/mediaplayer-notification.cpp

	media-player-status-changer.cpp
	mediaplayer-configuration-ui-handler.cpp
	mediaplayer-module.cpp
	mediaplayer-plugin-modules-factory.cpp
	mediaplayer-plugin-object.cpp
	mediaplayer.cpp
)

set (CONFIGURATION_FILES
	configuration/mediaplayer.ui
)

set (DATA_FILES
	data/mediaplayer.png
)

kadu_plugin (mediaplayer
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES docking
	PLUGIN_LIBRARIES ${DBUS_LIBS}
	PLUGIN_DATA_FILES ${DATA_FILES}
)
