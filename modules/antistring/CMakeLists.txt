set (SOURCES
	antistring.cpp)

set (MOC_SOURCES
	antistring.h)

set (TRANSLATION_SOURCES
	translations/antistring_pl.ts)

set (CONFIGURATION_FILES
	configuration/antistring.ui)

set (DATA_FILES
	data/ant_conditions.conf)

kadu_module (antistring
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

install (FILES ${DATA_FILES}
	DESTINATION share/kadu/modules/data/antistring
)
