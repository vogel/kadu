set (SOURCES
	history.cpp
	history_dialog.cpp
	history_module.cpp
	history_search_dialog.cpp
)

set (MOC_SOURCES
	history.h
	history_dialog.h
	history_module.h
	history_search_dialog.h
)

set (TRANSLATION_SOURCES
	translations/history_de.ts
	translations/history_fr.ts
	translations/history_it.ts
	translations/history_pl.ts
)

set (CONFIGURATION_FILES
	configuration/history.ui)

kadu_module (history
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_BUILDDEF HISTORY_BUILD
)
