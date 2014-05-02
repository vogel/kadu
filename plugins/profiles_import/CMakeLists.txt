project (profiles_import)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

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

set (CONFIGURATION_FILES
)

kadu_plugin (profiles_import
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	PLUGIN_DEPENDENCIES gadu_protocol history_migration
	PLUGIN_ADDITIONAL_QT_MODULES XmlPatterns
)
