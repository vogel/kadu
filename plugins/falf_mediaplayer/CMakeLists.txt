project (falf_mediaplayer)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

include_directories (${KADU_INCLUDE_DIR})
include_directories (${KADU_INCLUDE_DIR}/kadu-core)

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
