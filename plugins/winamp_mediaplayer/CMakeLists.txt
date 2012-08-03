project (winamp_mediaplayer)

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	winamp.cpp
	winamp-player-plugin.cpp
)

set (MOC_SOURCES
	winamp.h
	winamp-player-plugin.h
)

if (MINGW)
	set_source_files_properties (winamp.cpp PROPERTIES
		COMPILE_FLAGS -fno-strict-aliasing
	)
endif ()

kadu_plugin (winamp_mediaplayer
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_DEPENDENCIES mediaplayer
)
