project (history_migration)

# minimal required versions
cmake_minimum_required (VERSION 2.8)
set (QT_MIN_VERSION "4.7.0")
set (CMAKE_MIN_VERSION "2.8.0")

set (KADU_FIND_REQUIRED true)
set (KADU_INSTALLS_SDK TRUE)
include (FindKadu)

kadu_api_directories (plugins/history_migration
	gui/windows
	gui
	.
)

set (SOURCES
	gui/windows/history-import-window.cpp

	history-import-thread.cpp
	history-importer.cpp
	history-importer-chat-data.cpp
	history-importer-manager.cpp
	history-migration-actions.cpp
	history-migration-helper.cpp
	history-migration-plugin.cpp
)

set (MOC_SOURCES
	gui/windows/history-import-window.h

	history-migration-actions.h
	history-import-thread.h
	history-importer.h
	history-importer-chat-data.h
	history-importer-manager.h
	history-migration-plugin.h
)

set (CONFIGURATION_FILES
)

set (TRANSLATION_SOURCES
	translations/history_migration_cs.ts
	translations/history_migration_en.ts
	translations/history_migration_pl.ts
)

kadu_plugin (history_migration
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_TRANSLATION_SOURCES ${TRANSLATION_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	# TODO: make it more general, eg. gadu_protocol,history_storage
	PLUGIN_DEPENDENCIES gadu_protocol sql_history history
	PLUGIN_BUILDDEF HISTORY_MIGRATION_BUILD
)
