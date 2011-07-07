project (profiles_import)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.8.0")

set (KADU_FIND_REQUIRED true)
include (FindKadu)

set (SOURCES
	gui/windows/import-profile-window.cpp
	gui/windows/import-profiles-window.cpp

	profile-data-manager.cpp
	profile-importer.cpp
	profiles-import-actions.cpp
	profiles-import-plugin.cpp
)

set(MOC_SOURCES
	gui/windows/import-profile-window.h
	gui/windows/import-profiles-window.h

	profile-importer.h
	profiles-import-actions.h
	profiles-import-plugin.h
)

set (TRANSLATION_SOURCES
	translations/profiles_import_cs.ts
	translations/profiles_import_en.ts
	translations/profiles_import_pl.ts
)

set (CONFIGURATION_FILES
)

kadu_plugin (profiles_import
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES gadu_protocol history_migration
)
