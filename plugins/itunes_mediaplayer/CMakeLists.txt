project (itunes_mediaplayer)

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	itunes.cpp
	itunescontroller.cpp
	itunes-player-plugin.cpp
)

set (MOC_SOURCES
	itunes.h
	itunes-player-plugin.h
)

kadu_plugin (itunes_mediaplayer
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
)
