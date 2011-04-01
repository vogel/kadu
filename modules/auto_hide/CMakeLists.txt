set (SOURCES
	auto_hide.cpp
)

set (MOC_SOURCES
	auto_hide.h
)

set (TRANSLATION_SOURCES
	translations/auto_hide_en.ts
	translations/auto_hide_de.ts
	translations/auto_hide_fr.ts
	translations/auto_hide_it.ts
	translations/auto_hide_pl.ts
)

set (CONFIGURATION_FILES
	configuration/auto_hide.ui
)

kadu_module (auto_hide
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_DEPENDENCIES idle
)
