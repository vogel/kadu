set (SOURCES
	win32_sound.cpp
)

set (MOC_SOURCES
	win32_sound.h
)

set (TRANSLATION_SOURCES
#	translations/win32_sound_de.ts
#	translations/win32_sound_fr.ts
#	translations/win32_sound_it.ts
#	translations/win32_sound_pl.ts
)

set (CONFIGURATION_FILES
	configuration/win32_sound.ui
)

kadu_module (win32_sound
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_DEPENDENCIES sound
)
