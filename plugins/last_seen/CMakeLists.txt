project (last_seen)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.8.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	last-seen-plugin.cpp
	infos.cpp
	infos_dialog.cpp
)

set (MOC_SOURCES
	last-seen-plugin.h
	infos.h
	infos_dialog.h
)

kadu_plugin (last_seen
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
