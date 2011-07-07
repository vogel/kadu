project (auto_hide)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	auto_hide.cpp
)

set (MOC_SOURCES
	auto_hide.h
)

set (TRANSLATION_SOURCES
	translations/auto_hide_cs.ts
	translations/auto_hide_de.ts
	translations/auto_hide_en.ts
	translations/auto_hide_es_ES.ts
	translations/auto_hide_it.ts
	translations/auto_hide_pl.ts
)

set (CONFIGURATION_FILES
	configuration/auto_hide.ui
)

kadu_plugin (auto_hide
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES idle
)
