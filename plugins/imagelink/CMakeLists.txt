project (imagelink)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	configuration/image-link-configuration.cpp

	image-link.cpp
	image-link-plugin.cpp
)

set (MOC_SOURCES
	image-link.h
	image-link-plugin.h
)

set (CONFIGURATION_FILES
	data/configuration/image-link.ui
)

set (TRANSLATION_SOURCES
	translations/imagelink_cs.ts
	translations/imagelink_en.ts
	translations/imagelink_it.ts
	translations/imagelink_pl.ts
)

kadu_plugin (imagelink
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
