project (tabs)

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	tabs.cpp
	tabs-plugin.cpp
	gui/widgets/tab-bar.cpp
	gui/widgets/tab-widget.cpp
)

set (MOC_SOURCES
	tabs.h
	tabs-plugin.h
	gui/widgets/tab-bar.h
	gui/widgets/tab-widget.h
)

set (TRANSLATION_SOURCES
	translations/tabs_cs.ts
	translations/tabs_de.ts
	translations/tabs_en.ts
	translations/tabs_pl.ts
)

set (CONFIGURATION_FILES
	configuration/tabs.ui
)

kadu_plugin (tabs
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
