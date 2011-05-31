project (amarok1_mediaplayer)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

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
