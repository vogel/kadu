project (winamp_mediaplayer)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

include_directories (${KADU_INCLUDE_DIR}/kadu-core)

set (SOURCES
	winamp.cpp
	winamp-player-plugin.cpp
)

set (MOC_SOURCES
	winamp.h
	winamp-player-plugin.h
)

kadu_plugin (winamp_mediaplayer
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_DEPENDENCIES mediaplayer
)
