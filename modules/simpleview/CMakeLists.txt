set (SOURCES
	simpleview-module.cpp
	simpleview.cpp
	simpleview-config-ui.cpp
)

set (MOC_SOURCES
	simpleview.h
	simpleview-config-ui.h
)

set (TRANSLATION_SOURCES
	translations/simpleview_en.ts
	translations/simpleview_de.ts
	translations/simpleview_fr.ts
	translations/simpleview_it.ts
	translations/simpleview_pl.ts
)

set (CONFIGURATION_FILES
	configuration/simpleview.ui
)

kadu_module (simpleview
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_DEPENDENCIES docking
)

