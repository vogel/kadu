project (ext_sound)

find_package (Kadu REQUIRED)

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
