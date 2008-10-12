set (SOURCES
	mediaplayer.cpp
	mp_status_changer.cpp
)

set (MOC_SOURCES
	mediaplayer.h
	mp_status_changer.h
	player_commands.h
	player_info.h
)

set (TRANSLATION_SOURCES
	translations/mediaplayer_pl.ts
)

set (CONFIGURATION_FILES
	configuration/mediaplayer.ui)

kadu_module (mediaplayer
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
