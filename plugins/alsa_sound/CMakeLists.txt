project (alsa_sound)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)
message (STATUS "Found Kadu: " ${KADU_INCLUDE_DIR})

include (FindPkgConfig)

include_directories (${KADU_INCLUDE_DIR})
include_directories (${KADU_INCLUDE_DIR}/kadu-core)

set (SOURCES
	alsa-device.cpp
	alsa-player.cpp
	alsa-plugin.cpp
)

set (MOC_SOURCES
	alsa-player.h
	alsa-plugin.h
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

kadu_plugin (alsa_sound
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

pkg_search_module (ALSA REQUIRED alsa)

target_link_libraries (alsa_sound ${ALSA_LIBRARIES})
include_directories (${ALSA_INCLUDE_DIRS})
