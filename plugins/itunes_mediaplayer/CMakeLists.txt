project (itunes_mediaplayer)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

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
	PLUGIN_DEPENDENCIES mediaplayer
)
