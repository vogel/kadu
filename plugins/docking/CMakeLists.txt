project (docking)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)
find_package (KF5Notifications)

kadu_api_directories (plugins/docking
	.
)

set (SOURCES
	docking-configuration-provider.cpp
	docking-menu-action-repository.cpp
	docking-menu-handler.cpp
	docking-plugin.cpp
	docking-tooltip-handler.cpp
	docking.cpp
	status-notifier-item.cpp
	status-notifier-item-attention-blinker.cpp
)

set (CONFIGURATION_FILES
	data/configuration/docking.ui
)

kadu_plugin (docking
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_LIBRARIES "KF5::Notifications"
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
