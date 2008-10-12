set (SOURCES
	autostatus.cpp
	power_status_changer.cpp
)

set (MOC_SOURCES
	autostatus.h
	power_status_changer.h
)

set (TRANSLATION_SOURCES
	translations/autostatus_pl.ts)

set (CONFIGURATION_FILES
	configuration/autostatus.ui)

kadu_module (autostatus
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
