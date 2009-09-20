set (SOURCES
	sample-play-thread.cpp
	sample-record-thread.cpp
	sound.cpp
	sound-event.cpp
	sound-file.cpp
	sound-params.cpp
	sound-play-thread.cpp
	sound-slots.cpp
)

set (MOC_SOURCES
	sample-play-thread.h
	sample-record-thread.h
	sound.h
	sound-play-thread.h
	sound-slots.h
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
