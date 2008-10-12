set (SOURCES
	filtering.cpp
)

set (MOC_SOURCES
	filtering.h
)

set (TRANSLATION_SOURCES
	translations/filtering_pl.ts
)

set (CONFIGURATION_FILES
	configuration/filtering.ui)

kadu_module (filtering
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
