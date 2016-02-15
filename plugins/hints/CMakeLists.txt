project (hints)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	hint-manager.cpp
	hint-over-user-configuration-window.cpp
	hint.cpp
	hints-configuration-ui-handler.cpp
	hints-module.cpp
	hints-plugin-modules-factory.cpp
	hints-plugin-object.cpp
)

set (CONFIGURATION_FILES
	configuration/hint-over-user.ui
	configuration/hints.ui
	configuration/hints-advanced.ui
)

kadu_plugin (hints
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
