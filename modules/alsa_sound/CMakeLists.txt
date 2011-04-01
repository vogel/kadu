set (SOURCES
	alsa-device.cpp
	alsa-module.cpp
	alsa-player.cpp
)

set (MOC_SOURCES
	alsa-player.h
)

set (TRANSLATION_SOURCES
	translations/alsa_sound_en.ts
	translations/alsa_sound_de.ts
	translations/alsa_sound_fr.ts
	translations/alsa_sound_it.ts
	translations/alsa_sound_pl.ts
)

set (CONFIGURATION_FILES
	configuration/alsa_sound.ui
)

kadu_module (alsa_sound
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

pkg_search_module (ALSA REQUIRED alsa)

target_link_libraries (alsa_sound ${ALSA_LIBRARIES})
include_directories (${ALSA_INCLUDE_DIRS})
