set (SOURCES
	buddy-firewall-data.cpp
	firewall-configuration-ui-handler.cpp
	firewall-module.cpp
	firewall-notification.cpp
	firewall.cpp
)

set (MOC_SOURCES
	firewall-configuration-ui-handler.h
	firewall-notification.h
	firewall.h
)

set (TRANSLATION_SOURCES
	translations/firewall_pl.ts
)

set (CONFIGURATION_FILES
	configuration/firewall.ui
)

kadu_module (firewall
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_DEPENDENCIES history
)
