project (autoresponder)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	autoresponder.cpp
)

set (MOC_SOURCES
	autoresponder.h
)

set (TRANSLATION_SOURCES
	translations/autoresponder_cs.ts
	translations/autoresponder_de.ts
	translations/autoresponder_en.ts
	translations/autoresponder_it.ts
	translations/autoresponder_pl.ts
)

set (CONFIGURATION_FILES
	configuration/autoresponder.ui
)

kadu_plugin (autoresponder
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
