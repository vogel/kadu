set (SOURCES
	config-wizard-actions.cpp
	config-wizard-module.cpp
	wizard.cpp
)

set (MOC_SOURCES
	config-wizard-actions.h
	wizard.h
)

set (TRANSLATION_SOURCES
	translations/config_wizard_de.ts
	translations/config_wizard_fr.ts
	translations/config_wizard_it.ts
	translations/config_wizard_pl.ts
)

set (DATA_DIRECTORIES
	data/joi
	data/ronk2
)

kadu_module (config_wizard
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
)

install (DIRECTORY ${DATA_DIRECTORIES}
	DESTINATION share/kadu/modules/data/config_wizard
	PATTERN ".svn" EXCLUDE
)

# XXX
link_directories (${SNDFILE_LIBRARY_DIRS})
