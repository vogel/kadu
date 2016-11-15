project (mediaplayer)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

kadu_api_directories (plugins/mediaplayer
	.
	actions/
)

set (SOURCES
	actions/mediaplayer-menu-action.cpp
	actions/next-action.cpp
	actions/play-action.cpp
	actions/prev-action.cpp
	actions/stop-action.cpp
	actions/toggle-mediaplayer-statuses-action.cpp
	actions/volume-down-action.cpp
	actions/volume-up-action.cpp
	media-player-status-changer.cpp
	mediaplayer-configuration-ui-handler.cpp
	mediaplayer-module.cpp
	mediaplayer-notification-service.cpp
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
