project (ext_sound)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	ext-sound-module.cpp
	ext-sound-plugin-injector-factory.cpp
	ext-sound-plugin-object.cpp
	external-player.cpp
)

set (CONFIGURATION_FILES
	data/configuration/ext_sound.ui
)

kadu_plugin (ext_sound
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES sound
)
