set (SOURCES
	filedesc.cpp
)

set (MOC_SOURCES
	filedesc.h
)

set (TRANSLATION_SOURCES
	translations/filedesc_en.ts
	translations/filedesc_de.ts
	translations/filedesc_fr.ts
	translations/filedesc_it.ts
	translations/filedesc_pl.ts
)

set (CONFIGURATION_FILES
	configuration/filedesc.ui
)

kadu_module (filedesc
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
