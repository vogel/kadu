project (autoaway)

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	autoaway.cpp
	autoaway-status-changer.cpp
)

set (MOC_SOURCES
	autoaway.h
	autoaway-status-changer.h
)

set (CONFIGURATION_FILES
	configuration/autoaway.ui
)

kadu_plugin (autoaway
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES idle
)
