set (SOURCES
	tabs.cpp
	tabwidget.cpp
)

set (MOC_SOURCES
	tabs.h
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

kadu_module (tabs
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
