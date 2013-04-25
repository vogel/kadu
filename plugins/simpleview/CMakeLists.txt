project (simpleview)

find_package (Kadu REQUIRED)

set (SOURCES
	simpleview.cpp
	simpleview-config-ui.cpp
	simpleview-plugin.cpp
)

set (MOC_SOURCES
	simpleview.h
	simpleview-config-ui.h
	simpleview-plugin.h
)

set (CONFIGURATION_FILES
	configuration/simpleview.ui
)

kadu_plugin (simpleview
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES docking
)
