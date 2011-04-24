include_directories ("../../plugins/history")

set (SOURCES
	buddy-firewall-data.cpp
	firewall-configuration-ui-handler.cpp
	firewall-module.cpp
	firewall-notification.cpp
	firewall.cpp
)

set (MOC_SOURCES
	buddy-firewall-data.h
	firewall-configuration-ui-handler.h
	firewall-notification.h
	firewall.h
)

set (TRANSLATION_SOURCES
	translations/firewall_en.ts
	translations/firewall_de.ts
	translations/firewall_fr.ts
	translations/firewall_it.ts
	translations/firewall_pl.ts
)

set (CONFIGURATION_FILES
	configuration/firewall.ui
)

kadu_plugin (firewall
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES history
)
