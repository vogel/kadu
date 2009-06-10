set (SOURCES
	hint.cpp
	hint_manager.cpp
	hints_configuration_widget.cpp
	hints.cpp
)

set (MOC_SOURCES
	hint.h
	hint_manager.h
	hints_configuration_widget.h
)

set (TRANSLATION_SOURCES
	translations/hints_de.ts
	translations/hints_fr.ts
	translations/hints_it.ts
	translations/hints_pl.ts
)

set (CONFIGURATION_FILES
	configuration/hints.ui)

kadu_module (hints
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
