project (mediaplayer)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

kadu_api_directories (plugins/mediaplayer
	.
)

set (SOURCES
	mediaplayer.cpp
	mediaplayer-plugin.cpp
	mp_status_changer.cpp
)

set (MOC_SOURCES
	mediaplayer.h
	mediaplayer-plugin.h
	mp_status_changer.h
	player_commands.h
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


set (TRANSLATION_SOURCES
	translations/mediaplayer_cs.ts
	translations/mediaplayer_de.ts
	translations/mediaplayer_en.ts
	translations/mediaplayer_pl.ts
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
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_LIBRARIES ${DBUS_LIBS}
	PLUGIN_DATA_FILES ${DATA_FILES}
	PLUGIN_BUILDDEF MEDIAPLAYER_BUILD
)
