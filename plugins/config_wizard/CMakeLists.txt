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

	config-wizard-configuration-ui-handler.cpp
	config-wizard-plugin.cpp
)

set (MOC_SOURCES
	gui/widgets/config-wizard-choose-network-page.h
	gui/widgets/config-wizard-completed-page.h
	gui/widgets/config-wizard-page.h
	gui/widgets/config-wizard-profile-page.h
	gui/widgets/config-wizard-set-up-account-page.h
	gui/windows/config-wizard-window.h

	config-wizard-configuration-ui-handler.h
	config-wizard-plugin.h
)

set (CONFIGURATION_FILES
)

kadu_plugin (config_wizard
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
