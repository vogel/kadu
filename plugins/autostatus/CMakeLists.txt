project (autostatus)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	configuration/autostatus-configuration.cpp

	autostatus.cpp
	autostatus-actions.cpp
	autostatus-plugin.cpp
	autostatus-status-changer.cpp
)

set (MOC_SOURCES
	autostatus.h
	autostatus-actions.h
	autostatus-plugin.h
	autostatus-status-changer.h
)

set (CONFIGURATION_FILES
	data/configuration/autostatus.ui
)

kadu_plugin (autostatus
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
