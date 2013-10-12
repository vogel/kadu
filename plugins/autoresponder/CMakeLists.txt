project (autoresponder)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	autoresponder.cpp
	autoresponder-configuration.cpp
	autoresponder-configuration-ui-handler.cpp
	autoresponder-configurator.cpp
)

set (MOC_SOURCES
	autoresponder.h
	autoresponder-configuration-ui-handler.h
)

set (CONFIGURATION_FILES
	configuration/autoresponder.ui
)

kadu_plugin (autoresponder
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
