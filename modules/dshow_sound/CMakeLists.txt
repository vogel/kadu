set (SOURCES
	player_dshow.cpp
)

set (MOC_SOURCES
	player_dshow.h
)

kadu_plugin (dshow_sound
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_DEPENDENCIES sound
)
