set (SOURCES
	itunes.cpp
	itunescontroller.cpp
)

set (MOC_SOURCES
	itunes.h
)

kadu_plugin (itunes_mediaplayer
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
)
