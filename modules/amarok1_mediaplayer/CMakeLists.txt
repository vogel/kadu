set (SOURCES
	amarok.cpp
)

set (MOC_SOURCES
	amarok.h
)

kadu_plugin (amarok1_mediaplayer
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
)
