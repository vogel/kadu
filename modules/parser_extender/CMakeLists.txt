set (SOURCES
	parser_extender.cpp
)

set (TRANSLATION_SOURCES
	translations/parser_extender_de.ts
	translations/parser_extender_fr.ts
	translations/parser_extender_it.ts
	translations/parser_extender_pl.ts
)

set (CONFIGURATION_FILES
	configuration/parser_extender.ui
)

kadu_plugin (parser_extender
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
