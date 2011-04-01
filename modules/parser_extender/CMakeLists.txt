set (SOURCES
	parser_extender.cpp
)

set (TRANSLATION_SOURCES
	translations/parser_extender_en.ts
	translations/parser_extender_de.ts
	translations/parser_extender_fr.ts
	translations/parser_extender_it.ts
	translations/parser_extender_pl.ts
)

set (CONFIGURATION_FILES
	configuration/parser_extender.ui
)

kadu_module (parser_extender
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
