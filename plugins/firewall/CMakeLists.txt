project (firewall)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	firewall-configuration-ui-handler.cpp
	firewall-message-filter.cpp
	firewall-module.cpp
	firewall-notification-service.cpp
	firewall-plugin-modules-factory.cpp
	firewall-plugin-object.cpp
)

set (CONFIGURATION_FILES
	configuration/firewall.ui
)

kadu_plugin (firewall
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES history
)
