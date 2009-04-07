set (SOURCES
	voice.cpp
)

set (MOC_SOURCES
	voice.h
)

set (TRANSLATION_SOURCES
	translations/voice_de.ts
	translations/voice_fr.ts
	translations/voice_it.ts
	translations/voice_pl.ts
)

set (CONFIGURATION_FILES
	configuration/voice.ui)

kadu_module (voice
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

# XXX
link_directories (${SNDFILE_LIBRARY_DIRS})

add_subdirectory (libgsm)
