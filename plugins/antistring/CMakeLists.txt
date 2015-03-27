project (antistring)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	antistring.cpp
	antistring-configuration.cpp
	antistring-configuration-ui-handler.cpp
	antistring-notification.cpp
	antistring-plugin.cpp
)

set (CONFIGURATION_FILES
	configuration/antistring.ui
)

set (DATA_FILES
	data/ant_conditions.conf
)

kadu_plugin (antistring
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DATA_FILES ${DATA_FILES}
)
