project (autostatus)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	configuration/autostatus-configuration.cpp

	autostatus-actions.cpp
	autostatus-module.cpp
	autostatus-plugin-injector-factory.cpp
	autostatus-plugin-object.cpp
	autostatus-service.cpp
	autostatus-status-changer.cpp
)

set (CONFIGURATION_FILES
	data/configuration/autostatus.ui
)

kadu_plugin (autostatus
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
