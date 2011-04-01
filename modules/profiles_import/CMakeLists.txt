set (SOURCES
	gui/windows/import-profile-window.cpp
	gui/windows/import-profiles-window.cpp

	profile-data-manager.cpp
	profile-importer.cpp
	profiles-import-actions.cpp
	profiles-import-module.cpp
)

set(MOC_SOURCES
	gui/windows/import-profile-window.h
	gui/windows/import-profiles-window.h

	profile-importer.h
	profiles-import-actions.h
)

set (TRANSLATION_SOURCES
	translations/profiles_import_en.ts
	translations/profiles_import_de.ts
	translations/profiles_import_fr.ts
	translations/profiles_import_it.ts
	translations/profiles_import_pl.ts
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
