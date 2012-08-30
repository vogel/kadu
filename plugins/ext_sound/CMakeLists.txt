project (ext_sound)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	external-player.cpp
	ext-sound-plugin.cpp
)

set (MOC_SOURCES
	external-player.h
	ext-sound-plugin.h
)

set (CONFIGURATION_FILES
	configuration/ext_sound.ui
)

kadu_plugin (ext_sound
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES sound
)
