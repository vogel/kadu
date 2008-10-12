set (SOURCES
	player_external.cpp
)

set (MOC_SOURCES
	player_external.h
)

set (TRANSLATION_SOURCES
	translations/ext_sound_de.ts
	translations/ext_sound_fr.ts
	translations/ext_sound_it.ts
	translations/ext_sound_pl.ts
)

set (CONFIGURATION_FILES
	configuration/ext_sound.ui)

kadu_module (ext_sound
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

# XXX
link_directories (${SNDFILE_LIBRARY_DIRS})
