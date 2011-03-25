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

kadu_plugin (dsp_sound
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
