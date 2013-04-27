project (falf_mediaplayer)

find_package (Kadu REQUIRED)

set (SOURCES
	falf.cpp
)

set (MOC_SOURCES
	falf.h
)

kadu_plugin (falf_mediaplayer
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_DEPENDENCIES mediaplayer
)
