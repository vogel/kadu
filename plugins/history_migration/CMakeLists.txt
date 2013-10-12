project (history_migration)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

kadu_api_directories (plugins/history_migration
	gui/windows
	gui
	.
)

set (SOURCES
	gui/windows/history-import-window.cpp

	history-import-thread.cpp
	history-importer.cpp
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
	history-importer-manager.h
	history-migration-plugin.h
)

set (CONFIGURATION_FILES
)

kadu_plugin (history_migration
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_MOC_SOURCES ${MOC_SOURCES}
	PLUGIN_CONFIGURATION_FILES ${CONFIGURATION_FILES}
	# TODO: make it more general, eg. gadu_protocol,history_storage
	PLUGIN_DEPENDENCIES gadu_protocol sql_history history
)
