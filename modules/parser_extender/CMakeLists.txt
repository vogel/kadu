set (SOURCES
	parser_extender.cpp
)

set (TRANSLATION_SOURCES
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
