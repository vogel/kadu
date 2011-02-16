set (SOURCES
	antistring.cpp
	antistring-configuration.cpp
	antistring-configuration-ui-handler.cpp
	antistring-module.cpp
	antistring-notification.cpp
)

set (MOC_SOURCES
	antistring.h
	antistring-configuration-ui-handler.h
	antistring-notification.h
)

set (TRANSLATION_SOURCES
	translations/antistring_de.ts
	translations/antistring_fr.ts
	translations/antistring_it.ts
	translations/antistring_pl.ts
)

set (CONFIGURATION_FILES
	configuration/antistring.ui
)

set (DATA_FILES
	data/ant_conditions.conf
)

kadu_module (antistring
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

install (FILES ${DATA_FILES}
	DESTINATION share/kadu/modules/data/antistring
)
