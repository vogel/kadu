project (firewall)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.6.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

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
