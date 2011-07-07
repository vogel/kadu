project (autostatus)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

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

set (TRANSLATION_SOURCES
	translations/autostatus_cs.ts
	translations/autostatus_en.ts
	translations/autostatus_it.ts
	translations/autostatus_pl.ts
)

set (CONFIGURATION_FILES
	data/configuration/autostatus.ui
)

kadu_plugin (autostatus
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
