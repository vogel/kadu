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

kadu_module (mpris_mediaplayer
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
