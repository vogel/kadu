set (SOURCES
	gui/widgets/config-wizard-choose-network-page.cpp
	gui/widgets/config-wizard-completed-page.cpp
	gui/widgets/config-wizard-page.cpp
	gui/widgets/config-wizard-profile-page.cpp
	gui/widgets/config-wizard-set-up-account-page.cpp
	gui/windows/config-wizard-window.cpp

	config-wizard-configuration-ui-handler.cpp
	config-wizard-module.cpp
)

set (MOC_SOURCES
	gui/widgets/config-wizard-choose-network-page.h
	gui/widgets/config-wizard-completed-page.h
	gui/widgets/config-wizard-page.h
	gui/widgets/config-wizard-profile-page.h
	gui/widgets/config-wizard-set-up-account-page.h
	gui/windows/config-wizard-window.h

	config-wizard-configuration-ui-handler.h
)

set (CONFIGURATION_FILES
)

set (TRANSLATION_SOURCES
	translations/config_wizard_de.ts
	translations/config_wizard_fr.ts
	translations/config_wizard_it.ts
	translations/config_wizard_pl.ts
)

kadu_module (config_wizard
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_DEPENDENCIES sound
)

# XXX
link_directories (${SNDFILE_LIBRARY_DIRS})
