project (firewall)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	firewall-configuration-ui-handler.cpp
	firewall-notification.cpp
	firewall-plugin.cpp
	firewall.cpp
)

set (MOC_SOURCES
	firewall-configuration-ui-handler.h
	firewall-notification.h
	firewall-plugin.h
	firewall.h
)

set (CONFIGURATION_FILES
	configuration/firewall.ui
)

kadu_plugin (firewall
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES history
)
