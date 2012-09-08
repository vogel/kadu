project (imagelink)

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	configuration/image-link-configuration.cpp

	image-expander.cpp
	image-expander-dom-visitor-provider.cpp
	image-link-plugin.cpp
	video-expander.cpp
	video-expander-dom-visitor-provider.cpp
)

set (MOC_SOURCES
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
