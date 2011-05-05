project (mediaplayer)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

include_directories (${KADU_INCLUDE_DIR})
include_directories (${KADU_INCLUDE_DIR}/kadu-core)

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
	translations/mediaplayer_en.ts
	translations/mediaplayer_de.ts
	translations/mediaplayer_fr.ts
	translations/mediaplayer_it.ts
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
	PLUGIN_BUILDDEF MEDIAPLAYER_BUILD
)

install (FILES ${DATA_FILES}
	DESTINATION "share/kadu/modules/data/mediaplayer"
)

if (WIN32)
	set_target_properties (mediaplayer PROPERTIES COMPILE_FLAGS "-DMEDIAPLAYER_BUILD")
endif (WIN32)
