project (config_wizard)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

set (SOURCES
	gui/widgets/config-wizard-choose-network-page.cpp
	gui/widgets/config-wizard-completed-page.cpp
	gui/widgets/config-wizard-page.cpp
	gui/widgets/config-wizard-profile-page.cpp
	gui/widgets/config-wizard-set-up-account-page.cpp
	gui/windows/config-wizard-window.cpp

	config-wizard-actions.cpp
	config-wizard-module.cpp
	config-wizard-plugin-modules-factory.cpp
	config-wizard-plugin-object.cpp
	show-config-wizard-action.cpp
)

set (CONFIGURATION_FILES
)

kadu_plugin (config_wizard
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
