project (falf_mediaplayer)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

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
