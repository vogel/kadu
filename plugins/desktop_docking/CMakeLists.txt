project (desktop_docking)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	configuration/gui/desktop-dock-configuration-ui-handler.cpp
	gui/windows/desktop-dock-window.cpp
	desktop-docking-plugin.cpp
	desktop-dock.cpp
)

set (MOC_SOURCES
	configuration/gui/desktop-dock-configuration-ui-handler.h
	gui/windows/desktop-dock-window.h
	desktop-docking-plugin.h
	desktop-dock.h
)

set (TRANSLATION_SOURCES
	translations/desktop_docking_cs.ts
	translations/desktop_docking_de.ts
	translations/desktop_docking_en.ts
	translations/desktop_docking_es_ES.ts
	translations/desktop_docking_it.ts
	translations/desktop_docking_pl.ts
)

set (CONFIGURATION_FILES
	data/configuration/desktop_docking.ui
)

kadu_plugin (desktop_docking
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES docking
)
