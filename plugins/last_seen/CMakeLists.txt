project (last_seen)

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
