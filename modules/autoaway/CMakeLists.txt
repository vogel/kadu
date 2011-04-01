set (SOURCES
	autoaway.cpp
	autoaway-status-changer.cpp
)

set (MOC_SOURCES
	autoaway.h
	autoaway-status-changer.h
)

set (TRANSLATION_SOURCES
	translations/autoaway_en.ts
	translations/autoaway_de.ts
	translations/autoaway_fr.ts
	translations/autoaway_it.ts
	translations/autoaway_pl.ts
)

set (CONFIGURATION_FILES
	configuration/autoaway.ui
)

kadu_module (autoaway
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_DEPENDENCIES idle
)
