project (falf_mediaplayer)

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	falf.cpp
)

set (MOC_SOURCES
	falf.h
)

kadu_plugin (falf_mediaplayer
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
)
