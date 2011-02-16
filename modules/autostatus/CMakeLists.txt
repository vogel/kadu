set (SOURCES
	configuration/autostatus-configuration.cpp

	autostatus.cpp
	autostatus-actions.cpp
	autostatus-module.cpp
	autostatus-status-changer.cpp
)

set (MOC_SOURCES
	autostatus.h
	autostatus-actions.h
	autostatus-status-changer.h
)

set (TRANSLATION_SOURCES
	translations/autostatus_de.ts
	translations/autostatus_fr.ts
	translations/autostatus_it.ts
	translations/autostatus_pl.ts
)

set (CONFIGURATION_FILES
	data/configuration/autostatus.ui
)

kadu_module (autostatus
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
