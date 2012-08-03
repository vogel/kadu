project (filedesc)

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	filedesc-plugin.cpp
	filedesc.cpp
)

set (MOC_SOURCES
	filedesc-plugin.h
	filedesc.h
)

set (TRANSLATION_SOURCES
	translations/filedesc_cs.ts
	translations/filedesc_de.ts
	translations/filedesc_en.ts
	translations/filedesc_pl.ts
)

set (CONFIGURATION_FILES
	configuration/filedesc.ui
)

kadu_plugin (filedesc
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
