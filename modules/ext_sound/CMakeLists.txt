set (SOURCES
	external-player.cpp
	ext-sound-module.cpp
)

set (MOC_SOURCES
	external-player.h
)

set (TRANSLATION_SOURCES
	translations/ext_sound_en.ts
	translations/ext_sound_de.ts
	translations/ext_sound_fr.ts
	translations/ext_sound_it.ts
	translations/ext_sound_pl.ts
)

set (CONFIGURATION_FILES
	configuration/ext_sound.ui
)

kadu_module (ext_sound
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_DEPENDENCIES sound
)

# XXX
link_directories (${SNDFILE_LIBRARY_DIRS})
