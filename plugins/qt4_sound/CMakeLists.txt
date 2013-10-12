project (qt4_sound)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	qt4_sound.cpp
	qt4-sound-plugin.cpp
)

set (MOC_SOURCES
	qt4_sound.h
	qt4-sound-plugin.h
)

kadu_plugin (qt4_sound
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES sound
)
