project (last_seen)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

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
