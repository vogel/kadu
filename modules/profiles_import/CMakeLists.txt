set (SOURCES
	profile-importer.cpp
	profiles-import-actions.cpp
	profiles-import-module.cpp
)

set(MOC_SOURCES
	profile-importer.h
	profiles-import-actions.h
)

set (TRANSLATION_SOURCES
)

set (CONFIGURATION_FILES
)

kadu_module (profiles_import
	MODULE_SOURCES ${SOURCES}
	MODULE_MOC_SOURCES ${MOC_SOURCES}
	MODULE_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	MODULE_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	MODULE_DEPENDENCIES gadu_protocol history_migration
)
