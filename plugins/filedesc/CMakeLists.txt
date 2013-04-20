project (filedesc)

find_package (Kadu REQUIRED)

set (SOURCES
	filedesc-plugin.cpp
	filedesc.cpp
)

set (MOC_SOURCES
	filedesc-plugin.h
	filedesc.h
)

set (CONFIGURATION_FILES
	configuration/filedesc.ui
)

kadu_plugin (filedesc
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
