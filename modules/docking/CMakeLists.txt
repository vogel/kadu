set (SOURCES
	docking-module.cpp
	docking.cpp
)

set (MOC_SOURCES
	docking.h
)

set (TRANSLATION_SOURCES
	translations/docking_de.ts
	translations/docking_fr.ts
	translations/docking_it.ts
	translations/docking_pl.ts
)

set (CONFIGURATION_FILES
	data/configuration/docking.ui)

kadu_module (docking
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_BUILDDEF DOCKING_BUILD
)
