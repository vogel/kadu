project (mprisplayer_mediaplayer)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	gui/windows/mpris-player-dialog.cpp

	mpris-player-configuration-ui-handler.cpp
	mpris-player.cpp
	mpris-player-plugin.cpp
	track-info.cpp
)

set (MOC_SOURCES
	gui/windows/mpris-player-dialog.h

	mpris-player-configuration-ui-handler.h
	mpris-player.h
	mpris-player-plugin.h
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

	set (DBUS_LIBS
		${QT_QTDBUS_LIBRARY}
	)
endif (UNIX AND NOT APPLE)

set (CONFIGURATION_FILES
	configuration/mprisplayer_mediaplayer.ui
)

set (DATA_FILES
	data/mprisplayer-players.data
)

kadu_plugin (mprisplayer_mediaplayer
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DATA_FILES ${DATA_FILES}
)
