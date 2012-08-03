project (mediaplayer)

set (KADU_FIND_REQUIRED true)
set (KADU_INSTALLS_SDK TRUE)
include (FindKadu)

kadu_api_directories (plugins/mediaplayer
	.
)

set (SOURCES
	notify/mediaplayer-notification.cpp

	mediaplayer.cpp
	mediaplayer-plugin.cpp
	mp_status_changer.cpp
)

set (MOC_SOURCES
	notify/mediaplayer-notification.h

	mediaplayer.h
	mediaplayer-plugin.h
	mp_status_changer.h
	player_commands.h
)

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
	PLUGIN_DEPENDENCIES docking
	PLUGIN_LIBRARIES ${DBUS_LIBS}
	PLUGIN_DATA_FILES ${DATA_FILES}
	PLUGIN_BUILDDEF MEDIAPLAYER_BUILD
)
