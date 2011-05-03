set (SOURCES
	tabs.cpp
	tabs-plugin.cpp
	tabwidget.cpp
)

set (MOC_SOURCES
	tabs.h
	tabs-plugin.h
	tabwidget.h
)

set (TRANSLATION_SOURCES
	translations/tabs_en.ts
	translations/tabs_de.ts
	translations/tabs_fr.ts
	translations/tabs_it.ts
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
