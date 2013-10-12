project (imagelink)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	configuration/image-link-configuration.cpp
	configuration/image-link-configurator.cpp

	image-expander.cpp
	image-expander-dom-visitor-provider.cpp
	image-link-plugin.cpp
	video-expander.cpp
	video-expander-dom-visitor-provider.cpp
)

set (MOC_SOURCES
	configuration/image-link-configurator.h

	image-expander-dom-visitor-provider.h
	image-link-plugin.h
	video-expander-dom-visitor-provider.h
)

set (CONFIGURATION_FILES
	data/configuration/image-link.ui
)

kadu_plugin (imagelink
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
