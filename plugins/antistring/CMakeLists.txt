project (antistring)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	antistring.cpp
	antistring-configuration.cpp
	antistring-configuration-ui-handler.cpp
	antistring-notification.cpp
	antistring-plugin.cpp
)

set (MOC_SOURCES
	antistring.h
	antistring-configuration-ui-handler.h
	antistring-notification.h
	antistring-plugin.h
)

set (TRANSLATION_SOURCES
	translations/antistring_cs.ts
	translations/antistring_de.ts
	translations/antistring_en.ts
	translations/antistring_pl.ts
)

set (CONFIGURATION_FILES
	configuration/antistring.ui
)

set (DATA_FILES
	data/ant_conditions.conf
)

kadu_plugin (antistring
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DATA_FILES ${DATA_FILES}
)
