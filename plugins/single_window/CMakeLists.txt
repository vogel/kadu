project (single_window)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

include_directories (${KADU_INCLUDE_DIR}/kadu-core)

set (SOURCES
	single-window.cpp
	single-window-plugin.cpp
)

set (MOC_SOURCES
	single-window.h
	single-window-plugin.h
)

set (TRANSLATION_SOURCES
	translations/single_window_en.ts
	translations/single_window_de.ts
	translations/single_window_fr.ts
	translations/single_window_it.ts
	translations/single_window_pl.ts
)

set (CONFIGURATION_FILES
	configuration/single_window.ui
)

kadu_plugin (single_window
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES docking
)
