set (SOURCES
	autoresponder.cpp
)

set (MOC_SOURCES
	autoresponder.h
)

set (TRANSLATION_SOURCES
	translations/autoresponder_en.ts
	translations/autoresponder_de.ts
	translations/autoresponder_fr.ts
	translations/autoresponder_it.ts
	translations/autoresponder_pl.ts
)

set (CONFIGURATION_FILES
	configuration/autoresponder.ui
)

kadu_module (autoresponder
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
