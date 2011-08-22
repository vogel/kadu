project (gadu_protocol)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.8.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

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

set (TRANSLATION_SOURCES
	translations/config_wizard_cs.ts
	translations/config_wizard_en.ts
	translations/config_wizard_it.ts
	translations/config_wizard_pl.ts
)

kadu_plugin (config_wizard
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)
