project (imagelink)

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	configuration/image-link-configuration.cpp

	image-link.cpp
	image-link-plugin.cpp
)

set (MOC_SOURCES
	image-link.h
	image-link-plugin.h
)

set (CONFIGURATION_FILES
	data/configuration/image-link.ui
)

kadu_plugin (imagelink
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
