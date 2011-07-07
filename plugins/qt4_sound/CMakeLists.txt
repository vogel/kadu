project (qt4_sound)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	qt4_sound.cpp
	qt4-sound-plugin.cpp
)

set (MOC_SOURCES
	qt4_sound.h
	qt4-sound-plugin.h
)

set (TRANSLATION_SOURCES
	translations/qt4_sound_cs.ts
	translations/qt4_sound_en.ts
	translations/qt4_sound_pl.ts
)

kadu_plugin (qt4_sound
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES sound
)
