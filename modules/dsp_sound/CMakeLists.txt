set (SOURCES
	dsp_sound.cpp
)

set (MOC_SOURCES
	dsp_sound.h
)

set (TRANSLATION_SOURCES
	translations/dsp_sound_de.ts
	translations/dsp_sound_fr.ts
	translations/dsp_sound_it.ts
	translations/dsp_sound_pl.ts
)

set (CONFIGURATION_FILES
	configuration/dsp_sound.ui
)

kadu_module (dsp_sound
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
