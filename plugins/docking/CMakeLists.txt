project (docking)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)
find_package (KF5Notifications)

kadu_api_directories (plugins/docking
	.
)

set (SOURCES
	docking-plugin.cpp
	docking.cpp
	status-notifier-item.cpp
)

set (CONFIGURATION_FILES
	data/configuration/docking.ui
)

kadu_plugin (docking
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_LIBRARIES "KF5::Notifications"
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
