project (mprisplayer_mediaplayer)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	gui/windows/mpris-player-dialog.cpp

	mpris-player-configuration-ui-handler.cpp
	mpris-player-module.cpp
	mpris-player-plugin-injector-factory.cpp
	mpris-player-plugin-object.cpp
	mpris-player.cpp
	track-info.cpp
)

if (UNIX AND NOT APPLE)
	set (SOURCES ${SOURCES}
		mpris_controller.cpp
		mpris_mediaplayer.cpp
	)

	set (MOC_SOURCES ${MOC_SOURCES}
		mpris_controller.h
		mpris_mediaplayer.h
	)
endif ()

set (CONFIGURATION_FILES
	configuration/mprisplayer_mediaplayer.ui
)

set (DATA_FILES
	data/mprisplayer-players.data
)

kadu_plugin (mprisplayer_mediaplayer
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DATA_FILES ${DATA_FILES}
	PLUGIN_DEPENDENCIES mediaplayer
)
