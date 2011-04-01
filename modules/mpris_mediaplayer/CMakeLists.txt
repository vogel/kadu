set (SOURCES
	mpris.cpp
)

set (MOC_SOURCES
	mpris.h
)

set (TRANSLATION_SOURCES
	translations/mpris_mediaplayer_en.ts
	translations/mpris_mediaplayer_pl.ts
)

set (CONFIGURATION_FILES
	configuration/mpris_mediaplayer.ui
)

kadu_plugin (mpris_mediaplayer
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
