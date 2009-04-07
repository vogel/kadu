set (SOURCES
	player_dshow.cpp
)

set (MOC_SOURCES
	player_dshow.h
)

set (TRANSLATION_SOURCES
	translations/player_dshow_de.ts
	translations/player_dshow_fr.ts
	translations/player_dshow_it.ts
	translations/player_dshow_pl.ts
)

kadu_module (dshow_sound
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

# XXX
link_directories (${SNDFILE_LIBRARY_DIRS})
