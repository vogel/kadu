set (SOURCES
	sound.cpp
	sound_file.cpp
	sound_slots.cpp
)

set (MOC_SOURCES
	sound.h
	sound_file.h
	sound_slots.h
)

set (TRANSLATION_SOURCES
	translations/sound_de.ts
	translations/sound_fr.ts
	translations/sound_it.ts
	translations/sound_pl.ts
)

set (CONFIGURATION_FILES
	configuration/sound.ui)

kadu_module (sound
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_BUILDDEF SOUND_BUILD
)

pkg_search_module (SNDFILE REQUIRED sndfile>=1.0)
include_directories (${SNDFILE_INCLUDE_DIRS})
link_directories (${SNDFILE_LIBRARY_DIRS})

target_link_libraries (sound ${SNDFILE_LIBRARIES})
