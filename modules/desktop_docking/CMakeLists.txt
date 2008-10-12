set (SOURCES
	desktopdock.cpp
)

set (MOC_SOURCES
	desktopdock.h
)

set (TRANSLATION_SOURCES
	translations/desktop_docking_de.ts
	translations/desktop_docking_fr.ts
	translations/desktop_docking_it.ts
	translations/desktop_docking_pl.ts
)

set (CONFIGURATION_FILES
	configuration/desktop_docking.ui)

kadu_module (desktop_docking
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
