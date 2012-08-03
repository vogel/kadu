project (simpleview)

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	simpleview.cpp
	simpleview-config-ui.cpp
	simpleview-plugin.cpp
)

set (MOC_SOURCES
	simpleview.h
	simpleview-config-ui.h
	simpleview-plugin.h
)

set (TRANSLATION_SOURCES
	translations/simpleview_cs.ts
	translations/simpleview_en.ts
	translations/simpleview_pl.ts
)

set (CONFIGURATION_FILES
	configuration/simpleview.ui
)

kadu_plugin (simpleview
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES docking
)
