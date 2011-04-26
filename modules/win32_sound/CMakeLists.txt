set (SOURCES
	win32_sound.cpp
)

set (MOC_SOURCES
	win32_sound.h
)

set (TRANSLATION_SOURCES
#	translations/win32_sound_en.ts
#	translations/win32_sound_de.ts
#	translations/win32_sound_fr.ts
#	translations/win32_sound_it.ts
#	translations/win32_sound_pl.ts
)

set (CONFIGURATION_FILES
	configuration/win32_sound.ui
)

kadu_plugin (win32_sound
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES sound
)
