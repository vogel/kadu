project (autoresponder)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	autoresponder-configuration-ui-handler.cpp
	autoresponder-configuration.cpp
	autoresponder-configurator.cpp
	autoresponder-message-filter.cpp
	autoresponder-module.cpp
	autoresponder-plugin-injector-factory.cpp
	autoresponder-plugin-object.cpp
)

set (CONFIGURATION_FILES
	configuration/autoresponder.ui
)

kadu_plugin (autoresponder
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
