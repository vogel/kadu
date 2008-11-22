set (SOURCES
	macosx_docking.cpp
)

set (MOC_SOURCES
	macosx_docking.h
)

set (CONFIGURATION_FILES
	configuration/macosx_docking.ui)

set (TRANSLATION_SOURCES
	translations/macosx_docking_de.ts
	translations/macosx_docking_en.ts
	translations/macosx_docking_fr.ts
	translations/macosx_docking_it.ts
	translations/macosx_docking_pl.ts
)

kadu_module (macosx_docking
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
